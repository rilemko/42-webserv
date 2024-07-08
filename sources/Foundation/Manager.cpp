/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/08 13:17:59 by mconreau         ###   ########.fr       */
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
	this->_servers.clear();
	this->_pending.clear();
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

	// ==============================================================
	// TO REWORK:

	Server	*server;
	Route	*route;
	size_t	status;

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if ((server = this->_servers[i])->match(*req))
		{
			if ((status = server->check(*req)) != 200)
			{
				Response(fd).setStatus(status).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(server->errors[status] != "" ? Filesystem::get(server->errors[status]) : Template::error(status)).send();
				goto next;
			}
			else
			{
				for (size_t j = 0; j < server->routes.size(); j++)
				{
					if ((route = server->routes[j])->match(*req))
					{
						if ((status = route->check(*req)) != 200)
						{
							Response(fd).setStatus(status).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(server->errors[status] != "" ? Filesystem::get(server->errors[status]) : Template::error(status)).send();
							goto next;
						}
						else
						{
							if ((status = req->getStatus()) != 200)
							{
								Response(fd).setStatus(status).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(Template::error(status)).send();
								goto next;
							}
							if (route->upload != "")
							{
								if (!Filesystem::isDir(route->upload))
								{
									Response(fd).setStatus(500).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(server->errors[500] != "" ? Filesystem::get(server->errors[500]) : Template::error(500)).send();
									goto next;
								}
								
								map<string,string>	m = req->getUpload();

								for (map<string,string>::const_iterator it = m.begin(); it != m.end(); it++)
									Filesystem::set(route->upload + (*it).first, (*it).second);
							}
							
							if (route->rewrite.first != 0)
								Response(fd).setStatus(route->rewrite.first).setSessid(req->getCookie("sessid", String::rand(64))).addHeader("Location", route->rewrite.second).send();
							else if (route->passcgi.size())
							{
								const string	t = route->rooting + req->getTarget();

								if (Filesystem::isDir(route->rooting + req->getTarget()) && route->dindex.size() && Filesystem::isReg(route->rooting + req->getTarget() + route->dindex))
									Response(fd).send(Gateway().cgirun(*req, route->passcgi, this->_basedir + "/" + t + route->dindex));
								else if (Filesystem::isReg(route->rooting + req->getTarget()))
									Response(fd).setSessid(req->getCookie("sessid", String::rand(64))).send(Gateway().cgirun(*req, route->passcgi, this->_basedir + "/" + t));
								else
									Response(fd).setStatus(404).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
							}
							else if (Filesystem::isReg(route->rooting + req->getTarget()))
								Response(fd).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(Filesystem::get(route->rooting + req->getTarget())).send();
							else if (Filesystem::isDir(route->rooting + req->getTarget()))
							{
								if (route->dindex.size() && Filesystem::isReg(route->rooting + req->getTarget() + route->dindex))
									Response(fd).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(Filesystem::get(route->rooting + req->getTarget() + route->dindex)).send();
								else if (route->dirlst)
									Response(fd).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(Template::index(route->rooting + req->getTarget())).send();
								else
									Response(fd).setStatus(404).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
							}
							else
								Response(fd).setStatus(404).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
							goto next;
						}
					}
				}
				Response(fd).setStatus(404).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
				goto next;
			}
		}
	}
	Response(fd).setStatus(404).setSessid(req->getCookie("sessid", String::rand(64))).addPacket(Template::error(404)).send();
	next: ;

	// ==============================================================

	if (String::lowercase(req->getHeader("connection", "")) != "close")
		this->_clients[fd] = ::time(0);
	else
		::close(fd);

	delete req;
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
		if (now - (it->second) > 10)
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
