/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/11 23:58:49 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Application.hpp"

Application::Application(const string &config) :
	_epollfd(epoll_create(1)),
	_servers(Configuration(config.size() ? config : "config/default.cfg", this->_epollfd))
{
	if (this->_servers.size() && !config.size())
		Logger::warn("Default configuration file used.");
	else if (!this->_servers.size() && this->_epollfd == -1)
		this->abort("Failed to create epoll instance.");
	else if (!this->_servers.size() && this->_epollfd != -1)
		this->abort("No server to listen on.");
}

Application::Application(const Application &src) :
	Abortable(src)
{
	*this = src;
}

Application::~Application()
{
	Vector::free(this->_servers);
	this->_servers.clear();
	::close(this->_epollfd);
}

bool
Application::run()
{
	// Using "const int" instead of member variable to prevent program from evaluting the value each time and optimize execution
	// "hfd" is the higher fd opened by the program before listening (0,1,2 for standard fd + 3rd for epoll + nth for servers initial socket)
	const int			epollfd = this->_epollfd, hfd = this->_servers[this->_servers.size() - 1]->socket;
	int					cli, e, fd;
	epoll_event			event, events[32];
	sockaddr_in			remote;
	socklen_t			l;
	map<int,time_t>		clients;
	map<int,Request>	chunked;
	
	event.events = EPOLLIN; // Setup event to trigger epoll only when data is received, not when data is sended

	// ==========================
	// Main loop
	// ==========================

	while (this->_status) // While _status == true (may be set to false by Ctrl-C)
	{
		if ((e = ::epoll_wait(epollfd, events, 32, 2000)) == -1) // Wait until epoll trigger event or until 2000ms (for timeout check)
			return (::perror("epoll_wait"), false);
		for (int i = 0; i < e; i++) // For each triggered event from epoll (number of trigerred events is in "e")
		{
			if ((fd = events[i].data.fd) <= hfd) // If the "fd" triggered by epoll is one of the servers socket (<= "hfd"), it's a first connection from a new client
			{
				if ((cli = ::accept(fd, (sockaddr*) &remote, &l)) > -1) // Create a copy of the fd so as not to block the server initial socket
				{
					event.data.fd = cli;
					if (::epoll_ctl(epollfd, EPOLL_CTL_ADD, cli, &event) == -1) // Then add the copied fd "cli" to epoll
						::close(cli);
				}
			}
			else // Handle the data receveid on the socket "fd"
			{
				Request		req(fd);
				Response	res(fd);

				req.recv(); // Receive the data from the socket
				
				// =====================
				// HERE: TOUT SE PASSE ICI <<<<<<<<<<<<<<<<<<<<<<<<<<<
				// =====================

				res.setStatus(200);
				res.addPacket("<h1>Hello there!</h1>");
				res.send(); // Send the data to the socket

				// if (req.getHeader("connection", "keep-alive") != "close") // <= Use this for "keep-alive" by default with HTTP/1.1, commented for testing purpose only
				if (req.getHeader("connection", "") == "keep-alive") // <= Used for testing purpose only, use the above one in production
					clients[fd] = ::time(0); // Create or update the keep-alive fd timestamp...
				else
					::close(fd); // Or close the fd
			}
		}

		// ==========================
		// Timeout handling
		// ==========================

		const time_t	now = ::time(0);

		for (map<int,time_t>::iterator it = clients.begin(); it != clients.end();) // For each elem in keep-alive clients map...
		{
			if (now - (it->second) > 2) // If the time is exceeded (by 2 seconds for now)
			{
				// Close the fd and erase from the keep-alive clients map
				::close(it->first);
				clients.erase(it++);
			}
			else
				++it;
		}
	}

	return (true);
}

Application&
Application::operator=(const Application &rhs)
{
	Abortable::operator=(rhs);
	this->_epollfd = rhs._epollfd;
	this->_servers = rhs._servers;
	return (*this);
}
