/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/03 21:51:14 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Application.hpp"

bool	Application::_running = true;

Application::Application(const string &config, const string &basedir) :
	_basedir(basedir),
	_epollfd(epoll_create(1)),
	_servers(this->_epollfd != -1 ? Configuration(config.size() ? config : "config/default.cfg", this->_epollfd) : vector<Server*>())
{
	if (!this->_basedir.size())
		this->abort("Failed to find current directory.");
	else if (this->_epollfd == -1)
		this->abort("Failed to create epoll instance.");
	else if (!this->_servers.size())
		this->abort("No server to listen on.");
	else
	{
		if (!config.size())
			Logger::warn("Default configuration file used.");
		signal(SIGINT, &Application::stop);
	}
}

Application::Application(const Application &src) :
	Abortable(src)
{
	*this = src;
}

Application::~Application()
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
Application::stop(int i)
{
	(void) i;
	Application::_running = false;
}

bool
Application::run()
{
	const int		hfd = this->_servers.size() ? this->_servers.back()->socket : 0;
	epoll_event		events[16];

	for (int e, fd; this->_running && this->_status;)
	{
		if ((e = ::epoll_wait(this->_epollfd, events, 16, 2000)) == -1)
			return (this->abort(Application::_running ? "Failed to wait on epoll" : ""));
		for (int i = 0; i < e; i++)
		{
			if ((fd = events[i].data.fd) <= hfd)
				this->add(fd);
			else if (events[i].events == EPOLLIN)
				this->handle(fd);
			else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
				this->end(fd);
		}
		this->out();
	}
	return (this->_status);
}

void
Application::add(const int &fd)
{
	static epoll_event 	e = {EPOLLIN | EPOLLET | EPOLLRDHUP, {0}};
	static socklen_t	l = sizeof(sockaddr_in);
	static sockaddr_in	r;

	if ((e.data.fd = ::accept(fd, (sockaddr*) &r, &l)) > 0)
		if (::epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, e.data.fd, &e) == -1)
			::close(e.data.fd);
}

void
Application::end(const int &fd)
{
	Logger::info("Close the fd: " + String::tostr(fd));
	this->_clients.erase(fd);
	if (this->_pending.find(fd) != this->_pending.end())
	{
		delete this->_pending[fd];
		this->_pending.erase(fd);
	}
	::close(fd);
}


void
Application::handle(const int &fd)
{
	Request		*req = this->_pending.find(fd) != this->_pending.end() ? this->_pending[fd] : new Request(fd);

	req->recv();

	if (req->getStatus() == 101 || req->getStatus() == 102)
		return (this->_pending[fd] = req, (void) NULL);

	this->_pending.erase(fd);

	Server	*s;

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if ((s = this->_servers[i])->match(*req))
		{
			Response(fd).setStatus(200).addPacket("").send();
		}
	}
	Response(fd).setStatus(404).addPacket(Template::error(404)).send();
	next: ;

	if (String::lowercase(req->getHeader("connection", "")) != "close")
		this->_clients[fd] = ::time(0);
	else
		::close(fd);

	delete req;
}

/*
void
Application::handle(const int &fd)
{
	Request		*req = this->_pending.find(fd) != this->_pending.end() ? this->_pending[fd] : new Request(fd);

	req->recv();

	if (req->getStatus() == 101 || req->getStatus() == 102)
		return (cout << "a" << endl, this->_pending[fd] = req, (void) NULL);

	this->_pending.erase(fd);

	Server		*server;
	Route		*route;
	size_t		status;
	
	if ((status = req->getStatus()) != 200)
	{
		Response(fd).setStatus(status).addPacket(Template::error(req->getStatus())).send();
		return (delete req, this->end(fd), (void) NULL);
	}

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if ((server = this->_servers[i])->match(*req))
		{
			if ((status = server->check(*req)) != 200)
			{
				Response(fd).setStatus(status).addPacket(server->errors[status] != "" ? Filesystem::get(server->errors[status]) : Template::error(status)).send();
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
							Response(fd).setStatus(status).addPacket(server->errors[status] != "" ? Filesystem::get(server->errors[status]) : Template::error(status)).send();
							goto next;
						}
						else
						{
							cout << "-" << req->getPacket() << ":" << req->getStatus() << "-" << endl;

							if (route->rewrite.first != 0)
								Response(fd).setStatus(route->rewrite.first).addHeader("Location", route->rewrite.second).send();
							else if (route->passcgi.size())
							{
								const string	t = route->rooting + req->getTarget();
								cout << this->_basedir + "/" + t << endl;
								if (Filesystem::isDir(route->rooting + req->getTarget()) && route->dindex.size() && Filesystem::isReg(route->rooting + req->getTarget() + route->dindex))
									Response(fd).send(Gateway().cgirun(*req, route->passcgi, this->_basedir + "/" + t + route->dindex));
								else if (Filesystem::isReg(route->rooting + req->getTarget()))
									Response(fd).send(Gateway().cgirun(*req, route->passcgi, this->_basedir + "/" + t));
								else
									Response(fd).setStatus(404).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
							}
							else if (Filesystem::isReg(route->rooting + req->getTarget()))
								Response(fd).addPacket(Filesystem::get(route->rooting + req->getTarget())).send();
							else if (Filesystem::isDir(route->rooting + req->getTarget()))
							{
								if (route->dindex.size() && Filesystem::isReg(route->rooting + req->getTarget() + route->dindex))
									Response(fd).addPacket(Filesystem::get(route->rooting + req->getTarget() + route->dindex)).send();
								else if (route->dirlst)
									Response(fd).addPacket(Template::index(route->rooting + req->getTarget())).send();
								else
									Response(fd).setStatus(404).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
							}
							else
								Response(fd).setStatus(404).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
							goto next;
						}
					}
				}
				Response(fd).setStatus(404).addPacket(server->errors[404] != "" ? Filesystem::get(server->errors[404]) : Template::error(404)).send();
				goto next;
			}
		}
	}
	if ((status = req->getStatus()) != 200)
	{
		Response(fd).setStatus(status).addPacket(Template::error(req->getStatus())).send();
		if (this->_pending.find(fd) != this->_pending.end())
			this->_pending.erase(fd);
	}
	//else
	Response(fd).setStatus(404).addPacket(Template::error(404)).send();
	next: ;

	//if (String::lowercase(req->getHeader("connection", "keep-alive")) == "keep-alive") // <= Used for testing purpose only, use the above one in production
	if (String::lowercase(req->getHeader("connection", "")) == "keep-alive") // <= Used for testing purpose only, use the above one in production
		this->_clients[fd] = ::time(0); // Create or update the keep-alive fd timestamp...
	else
		::close(fd); // Or close the fd
	if (req->getStatus() != 101 && req->getStatus() != 102)
		delete req;
}
*/

void
Application::out()
{
	const time_t	now = ::time(0);

	for (map<const int,time_t>::iterator it = this->_clients.begin(); it != this->_clients.end();)
	{
		if (now - (it->second) > 10)
		{
			Logger::info("Timeout for: " + String::tostr(it->first));
			this->end((it++)->first);
		}
		else
			++it;
	}
}

Application&
Application::operator=(const Application &rhs)
{
	Abortable::operator=(rhs);
	this->_clients = rhs._clients;
	this->_epollfd = rhs._epollfd;
	this->_pending = rhs._pending;
	this->_servers = rhs._servers;
	return (*this);
}
