/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/26 19:22:41 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Manager.hpp"

Manager::Manager(const int &epollfd, const string &basedir, const vector<Server*> &servers) :
	_basedir(basedir),
	_epollfd(epollfd),
	_servers(servers)
{
}

Manager::Manager(const Manager &src)
{
	*this = src;
}

Manager::~Manager()
{
	Map<const int,time_t  >::iterKey(this->_clients, &::close);
	Map<const int,Request*>::iterKey(this->_pending, &::close);
	Map<const int,Request*>::freeVal(this->_pending);
	Vector::free(this->_servers);
	this->_clients.clear();
	this->_pending.clear();
	this->_servers.clear();
	::close(this->_epollfd);
}

void
Manager::recv(const int &fd)
{
	Request				*req = this->_pending.find(fd) != this->_pending.end() ? this->_pending[fd] : new Request(fd);
	static epoll_event 	e = {EPOLLOUT | EPOLLET | EPOLLRDHUP, {0}};

	req->recv();
	this->_pending[fd] = req;

	if (req->getStatus() == 101 || req->getStatus() == 102)
		return (this->_clients[fd] = ::time(0), (void) NULL);

	e.data.fd = fd;

	if (::epoll_ctl(this->_epollfd, EPOLL_CTL_MOD, e.data.fd, &e) == -1)
		this->end(e.data.fd);
}

void
Manager::send(const int &fd)
{
	Request				*req = this->_pending.find(fd) != this->_pending.end() ? this->_pending[fd] : new Request(fd);
	static epoll_event 	e = {EPOLLIN | EPOLLET | EPOLLRDHUP, {0}};

	this->_clients.erase(fd);
	this->_pending.erase(fd);

	Response	res(fd);
	Server		*srv;
	Route		*rte;

	res.setSessid(req->getCookie("sessid", String::rand(32)));

	for (size_t i = 0, ss = this->_servers.size(), s; i < ss; i++)
	{
		if ((srv = this->_servers[i])->match(*req))
		{
			if ((s = req->getStatus()) != 200 || (s = srv->check(*req)) != 200)
			{
				this->error(res, *srv, s);
				goto next;
			}

			for (size_t i = 0, rs = srv->routes.size(); i < rs; i++)
			{
				if ((rte = srv->routes[i])->match(*req))
				{
					res.addHeader("Allow", String::join(rte->method, ", "));

					if ((s = rte->check(*req)) != 200)
						this->error(res, *srv, s);
					else if (rte->rewrite.first != 0)
						res.setStatus(rte->rewrite.first).addHeader("Location", rte->rewrite.second).send();
					else
					{
						const string	r = rte->rooting != "" ? rte->rooting : this->_basedir;
						const string	t = String::rtrim(rte->target, "*");
						const string	target = Filesystem::normalize(r + (rte->alias.size() && req->getTarget().find(t) == 0 ? rte->alias + req->getTarget().substr(t.size()) : req->getTarget()));

						if (req->getMethod() == "DELETE" && !rte->passcgi.size())
						{
							Filesystem::remove(target);
							this->error(res, *srv, 200);
							goto next;
						}

						if (rte->upload != "" && (req->getMethod() == "POST" || req->getMethod() == "PUT" || req->getMethod() == "PATCH"))
						{
							if (!Filesystem::isDir(rte->upload) || !Filesystem::isWritable(rte->upload))
							{
								this->error(res, *srv, 500);
								goto next;
							}
							
							map<string,string>	m = req->getUpload();

							for (map<string,string>::const_iterator it = m.begin(); it != m.end(); it++)
								Filesystem::set(rte->upload + (*it).first, (*it).second);
						}

						if (rte->passcgi.size())
						{
							if (!Filesystem::isReg(rte->passcgi))
								this->error(res, *srv, 502);
							else
							{
								Gateway	cgi;

								if (rte->dindex.size() && Filesystem::isReg(target + rte->dindex))
								{
									if ((s = cgi.run(*req, rte->passcgi, (target.find(this->_basedir) == 0 ? target : this->_basedir + "/" + target) + rte->dindex)) != 200)
										this->error(res, *srv, s);
									else
										res.send(cgi.getPacket());
								}
								else if (Filesystem::isReg(target))
								{
									if ((s = cgi.run(*req, rte->passcgi, target.find(this->_basedir) == 0 ? target : this->_basedir + "/" + target)) != 200)
										this->error(res, *srv, s);
									else
										res.send(cgi.getPacket());
								}
								else
									this->error(res, *srv, 404);
							}
						}
						else if (Filesystem::isReg(target))
						{
							if (Filesystem::isReadable(target))
								res.setMimeType(Filesystem::mimetype(target)).addPacket(Filesystem::get(target)).send();
							else
								this->error(res, *srv, 403);
						}
						else if (Filesystem::isDir(target))
						{
							if (!Filesystem::isReadable(target))
								this->error(res, *srv, 403);
							else if (rte->dindex.size() && Filesystem::isReg(target + rte->dindex))
								res.setMimeType(Filesystem::mimetype(rte->dindex)).addPacket(Filesystem::get(target + rte->dindex)).send();
							else if (rte->dirlst)
								res.addPacket(Template::index(target, req->getTarget())).send();
							else
								this->error(res, *srv, 404);
						}
						else
							this->error(res, *srv, 404);
					}
					goto next;
				}
			}
			this->error(res, *srv, 404);
			goto next;
		}
	}
	res.setStatus(404).addPacket(Template::error(404)).send();
	next: ;
	
	e.data.fd = fd;
	
	if (::epoll_ctl(this->_epollfd, EPOLL_CTL_MOD, e.data.fd, &e) == -1)
		this->end(e.data.fd);

	if (String::lowercase(req->getHeader("connection", "")) != "close")
		this->_clients[fd] = ::time(0);
	else
	{
		Logger::logs("Close connection: " + String::tostr(fd));
		this->end(fd);
	}

	delete req;
}

void
Manager::error(Response &response, Server &server, const size_t &status)
{
		return (server.errors[status] != "" && Filesystem::isReg(server.errors[status])
			? response.setStatus(status).setMimeType(Filesystem::mimetype(server.errors[status])).addPacket(Filesystem::get(server.errors[status])).send()
			: response.setStatus(status).addPacket(Template::error(status)).send());
}

void
Manager::add(const int &fd)
{
	static epoll_event 	e = {EPOLLIN | EPOLLET | EPOLLRDHUP, {0}};
	static socklen_t	l = sizeof(sockaddr_in);
	static sockaddr_in	r;

	if ((e.data.fd = ::accept(fd, (sockaddr*) &r, &l)) > 0)
	{
		Logger::logs("New connection: " + String::tostr(e.data.fd));
		if (::epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, e.data.fd, &e) == -1)
			::close(e.data.fd);
	}
}

void
Manager::end(const int &fd)
{
	Logger::logs("Close connection: " + String::tostr(fd));
	this->_clients.erase(fd);
	if (this->_pending.find(fd) != this->_pending.end())
	{
		delete this->_pending[fd];
		this->_pending.erase(fd);
	}
	::epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, fd, NULL);
	::close(fd);
}

void
Manager::out()
{
	const time_t	now = ::time(0);

	for (map<const int,time_t>::iterator it = this->_clients.begin(); it != this->_clients.end();)
	{
		map<const int,time_t>::iterator c = it;
		it++;

		if (now - (c->second) > 5)
			this->end(c->first);
	}
}

Manager&
Manager::operator=(const Manager &rhs)
{
	this->_basedir = rhs._basedir;
	this->_clients = rhs._clients;
	this->_epollfd = rhs._epollfd;
	this->_pending = rhs._pending;
	this->_servers = rhs._servers;
	return (*this);
}
