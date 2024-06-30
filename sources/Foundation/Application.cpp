/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/30 12:54:17 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Application.hpp"
#include "Gateway/Gateway.hpp"

bool	Application::_running = true;

Application::Application(const string &config) :
	_epollfd(epoll_create(1)),
	_servers(this->_epollfd != -1 ? Configuration(config.size() ? config : "config/default.cfg", this->_epollfd) : vector<Server*>())
{
	if (this->_epollfd == -1)
		this->abort("Failed to create epoll instance.");
	else if (!this->_servers.size())
		this->abort("No server to listen on.");
	else if (!config.size())
		Logger::warn("Default configuration file used.");
	signal(SIGINT, &Application::stop);
}

Application::Application(const Application &src) :
	Abortable(src)
{
	*this = src;
}

Application::~Application()
{
	Map<const int,time_t>::iterKey(this->_clients, &::close);
	Vector::free(this->_servers);
	this->_chunked.clear();
	this->_clients.clear();
	this->_servers.clear();
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

	for (int e, fd; this->_running;) // While _status == true (may be set to false by Ctrl-C)
	{
		if ((e = ::epoll_wait(this->_epollfd, events, 16, 2000)) == -1) // Wait until epoll trigger event or until 2000ms (for timeout check)
			return (this->abort(Application::_running ? "Failed to wait on epoll" : ""));
		for (int i = 0; i < e; i++) // For each triggered event from epoll (number of trigerred events is in "e")
		{
			if ((fd = events[i].data.fd) <= hfd) // If the "fd" triggered by epoll is one of the servers socket (<= "hfd"), it's a first connection from a new client
				this->add(fd);
			else // Handle the data receveid on the socket "fd"
				this->handle(fd);
		}
		this->out();
	}
	return (this->_status);
}

void
Application::add(const int &fd)
{
	static epoll_event 	e = {EPOLLIN, {0}};
	static sockaddr_in	r;
	socklen_t			l = sizeof(sockaddr_in);

	if ((e.data.fd = ::accept(fd, (sockaddr*) &r, &l)) > -1)
		if (::epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, e.data.fd, &e) == -1)
			::close(e.data.fd);
}

void
Application::end(const int &fd)
{
	if (this->_chunked.find(fd) != this->_chunked.end())
	{
		delete this->_chunked[fd];
		this->_chunked.erase(fd);
	}
	this->_clients.erase(fd);
	::close(fd);
}

void
Application::handle(const int &fd)
{
	Request		*req = this->_chunked.find(fd) != this->_chunked.end() ? this->_chunked[fd] : new Request(fd);
	Server		*server;
	Route		*route;
	size_t		status;

	req->recv();
	
	if (req->getHeader("transfer-encoding", "") == "chunked" && this->_chunked.find(fd) == this->_chunked.end())
	{
		this->_chunked[fd] = req;
		return ;
	}

	if ((status = req->getStatus()) == 1)
		return ;
	
	this->_chunked.erase(fd);

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
						if ((status = req->getStatus()) != 200 || (status = route->check(*req)) != 200)
						{
							Response(fd).setStatus(status).addPacket(server->errors[status] != "" ? Filesystem::get(server->errors[status]) : Template::error(status)).send();
							goto next;
						}
						else
						{
							/* =======================
							/ ICI, SERVER ET ROUTE TROUVÉ, LA REQUETE EST CONSTRUITE ET ENVOYÉ
							/ ======================= */

							//cout << "-" << req->getPacket() << "-" << endl;

							if (route->rewrite.first != 0)
								Response(fd).setStatus(route->rewrite.first).addHeader("Location", route->rewrite.second).send();
							else if (route->passcgi.size())
							{
								if (Filesystem::isDir(route->rooting + req->getTarget()) && route->dindex.size() && Filesystem::isReg(route->rooting + req->getTarget() + route->dindex))
									req->setTarget(route->rooting + req->getTarget() + route->dindex);
								Response(fd).send(Gateway().cgirun(*req, route->passcgi));
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
		if (this->_chunked.find(fd) != this->_chunked.end())
			this->_chunked.erase(fd);
	}
	else
		Response(fd).setStatus(404).addPacket(Template::error(404)).send();
	next: ;

	// if (String::lowercase(req->getHeader("connection", "keep-alive")) != "close") // <= Use this for "keep-alive" by default with HTTP/1.1, commented for testing purpose only
	if (String::lowercase(req->getHeader("connection", "")) == "keep-alive") // <= Used for testing purpose only, use the above one in production
		this->_clients[fd] = ::time(0); // Create or update the keep-alive fd timestamp...
	else
		::close(fd); // Or close the fd
	delete req;
}

void
Application::out()
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

Application&
Application::operator=(const Application &rhs)
{
	Abortable::operator=(rhs);
	this->_clients = rhs._clients;
	this->_epollfd = rhs._epollfd;
	this->_servers = rhs._servers;
	return (*this);
}
