/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/21 21:41:15 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Application.hpp"

bool	Application::running = true;

Application::Application(const string &cfgpath, const string &basedir) :
	_basedir(basedir),
	_cfgpath(cfgpath),
	_epollfd(-1)
{
	if (!basedir.size())
		this->abort("Failed to find current directory.");
	else if (!cfgpath.size())
		Logger::warn("No configuration file provided. Default configuration file used.");
	signal(SIGINT, &Application::end);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
}

Application::Application(const Application &src) :
	Abortable(src)
{
	*this = src;
}

Application::~Application()
{
	this->_servers.clear();
	::close(this->_epollfd);
}

bool
Application::run()
{
	if (!this->_status)
		return (false);

	if ((this->_epollfd = ::epoll_create(1)) == -1)
		return (this->abort("Failed to create epoll instance"));

	if (!(this->_servers = Configuration(this->_cfgpath.size() ? this->_cfgpath : "config/default.cfg", this->_epollfd)).size())
		return (this->abort("No servers available to listen on"));

	Manager			manager(this->_epollfd, this->_basedir, this->_servers);
	const int		hfd = this->_servers.back()->socket;
	epoll_event		events[64];

	this->_servers.clear();

	for (int e, fd; this->running && this->_status;)
	{
		if ((e = ::epoll_wait(this->_epollfd, events, 64, 5000)) == -1)
			return (Application::running ? this->abort("Failed to wait on epoll") : true);
		for (int i = 0; i < e; i++)
		{
			if ((fd = events[i].data.fd) <= hfd)
				manager.add(fd);
			else if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
				manager.end(fd);
			else if (events[i].events == EPOLLOUT)
				manager.send(fd);
			else if (events[i].events == EPOLLIN)
				manager.recv(fd);
		}
		manager.out();
	}
	return (this->_status);
}

void
Application::end(int i)
{
	if (i == SIGINT)
		Application::running = false;
}

Application&
Application::operator=(const Application &rhs)
{
	Abortable::operator=(rhs);
	return (*this);
}
