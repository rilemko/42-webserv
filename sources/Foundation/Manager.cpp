/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 10:22:13 by mconreau         ###   ########.fr       */
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
	Map<const int,time_t>::iterKey(this->_clients, &::close);
	Map<const int,Request*>::iterKey(this->_pending, &::close);
	Vector::free(this->_servers);
	this->_clients.clear();
	this->_pending.clear();
	this->_servers.clear();
	::close(this->_epollfd);
}

void
Manager::run(const int &fd)
{
	Request		*req = this->_pending.find(fd) != this->_pending.end() ? this->_pending[fd] : new Request(fd);

	req->recv();

	if (req->getStatus() == 101 || req->getStatus() == 102)
		return (this->_pending[fd] = req, (void) NULL);

	this->_pending.erase(fd);

	Response	res(fd);
	Server		*srv;
	Route		*rte;

	res.setSessid(req->getCookie("sessid", String::rand(32, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789?!")));

	for (size_t i = 0, ss = this->_servers.size(), s; i < ss; i++)
	{
		if ((srv = this->_servers[i])->match(*req))
		{
			if ((s = req->getStatus()) != 200 || (s = srv->check(*req)) != 200)
			{
				this->error(res, s, srv->errors[s] != "" ? Filesystem::get(srv->errors[s]) : Template::error(s));
				goto next;
			}

			for (size_t i = 0, rs = srv->routes.size(); i < rs; i++)
			{
				if ((rte = srv->routes[i])->match(*req))
				{
					if ((s = rte->check(*req)) != 200)
					{
						if (s == 405) res.addHeader("Allow", String::join(rte->method, ", "));
						this->error(res, s, srv->errors[s] != "" ? Filesystem::get(srv->errors[s]) : Template::error(s));
					}
					else if (rte->rewrite.first != 0)
						res.setStatus(rte->rewrite.first).addHeader("Location", rte->rewrite.second).send();
					else
					{
						const string	target = rte->rooting + (req->getTarget().find(rte->target) == 0 ? rte->alias + req->getTarget().substr(rte->target.size()) : req->getTarget());
						cout << rte->target << " : " << rte->alias << " : " << req->getTarget() << " : " << target << endl;

						if (rte->upload != "")
						{
							if (!Filesystem::isDir(rte->upload))
							{
								this->error(res, 500, srv->errors[500] != "" ? Filesystem::get(srv->errors[500]) : Template::error(500));
								goto next;
							}
							
							map<string,string>	m = req->getUpload();

							for (map<string,string>::const_iterator it = m.begin(); it != m.end(); it++)
								Filesystem::set(rte->upload + (*it).first, (*it).second);
						}

						if (rte->passcgi.size())
						{
							if (rte->dindex.size() && Filesystem::isReg(target + rte->dindex))
								res.send(Gateway().run(*req, rte->passcgi, this->_basedir + "/" + target + rte->dindex));
							else if (Filesystem::isReg(target))
								res.send(Gateway().run(*req, rte->passcgi, this->_basedir + "/" + target));
							else
								this->error(res, 404, srv->errors[404] != "" ? Filesystem::get(srv->errors[404]) : Template::error(404));
						}
						else if (Filesystem::isReg(target))
							res.setMimeType(Filesystem::mimetype(target)).addPacket(Filesystem::get(target)).send();
						else if (Filesystem::isDir(target))
						{
							if (rte->dindex.size() && Filesystem::isReg(target + rte->dindex))
								res.setMimeType(Filesystem::mimetype(rte->dindex)).addPacket(Filesystem::get(target + rte->dindex)).send();
							else if (rte->dirlst)
								res.addPacket(Template::index(target)).send();
							else
								this->error(res, 404, srv->errors[404] != "" ? Filesystem::get(srv->errors[404]) : Template::error(404));
						}
						else
							this->error(res, 404, srv->errors[404] != "" ? Filesystem::get(srv->errors[404]) : Template::error(404));
					}
					goto next;
				}
			}
			this->error(res, 404, srv->errors[404] != "" ? Filesystem::get(srv->errors[404]) : Template::error(404));
			goto next;
		}
	}
	this->error(res, 404, Template::error(404));
	next: ;

	if (String::lowercase(req->getHeader("connection", "")) != "close")
		this->_clients[fd] = ::time(0);
	else
		::close(fd);

	delete req;
}

void
Manager::error(Response &response, const size_t &status, const string &packet)
{
	response.setStatus(status).addPacket(packet).send();
}

void
Manager::add(const int &fd)
{
	static epoll_event 	e = {EPOLLIN | EPOLLET | EPOLLRDHUP, {0}};
	static socklen_t	l = sizeof(sockaddr_in);
	static sockaddr_in	r;

	if ((e.data.fd = ::accept(fd, (sockaddr*) &r, &l)) > 0)
		if (::epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, e.data.fd, &e) == -1)
			::close(e.data.fd);
}

void
Manager::end(const int &fd)
{
	Logger::info("Close the connection: " + String::tostr(fd));
	this->_clients.erase(fd);
	if (this->_pending.find(fd) != this->_pending.end())
	{
		delete this->_pending[fd];
		this->_pending.erase(fd);
	}
	::close(fd);
}

void
Manager::out()
{
	const time_t	now = ::time(0);

	for (map<const int,time_t>::iterator it = this->_clients.begin(); it != this->_clients.end();)
	{
		if (now - (it->second) > 15)
			this->end((it++)->first);
		else
			++it;
	}
}

Manager&
Manager::operator=(const Manager &rhs)
{
	this->_clients = rhs._clients;
	this->_epollfd = rhs._epollfd;
	this->_pending = rhs._pending;
	this->_servers = rhs._servers;
	return (*this);
}
