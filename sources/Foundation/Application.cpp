/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/07 18:35:36 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Application.hpp"

bool	Application::_running = true;

Application::Application(const string &cfgpath, const string &basedir) :
	_basedir(basedir),
	_cfgpath(cfgpath)
{
	if (!basedir.size())
		this->abort("Failed to find current directory.");
	else if (!cfgpath.size())
		Logger::warn("No configuration file provided. Default configuration file used.");
	signal(SIGINT, &Application::end);
}

Application::Application(const Application &src) :
	Abortable(src)
{
	*this = src;
}

Application::~Application()
{
}

void
Application::end(int i)
{
	(void) i;
	Application::_running = false;
}

bool
Application::run()
{
	if (!this->_status)
		return (false);
	
	const int		epollfd = ::epoll_create(1);

	if (epollfd == -1)
		return (this->abort("Failed to create epoll instance."));

	vector<Server*>	servers = Configuration(this->_cfgpath.size() ? this->_cfgpath : "config/default.cfg", epollfd);

	if (!servers.size())
		return (this->abort("No server to listen on."));

	epoll_event		events[16];
	const int		hfd = servers.back()->socket;
	Manager			manager(epollfd, this->_basedir, servers);

	servers.clear();

	for (int e, fd; this->_running && this->_status;)
	{
		if ((e = ::epoll_wait(epollfd, events, 16, 2000)) == -1)
			return (this->abort(Application::_running ? "Failed to wait on epoll" : ""));
		for (int i = 0; i < e; i++)
		{
			if ((fd = events[i].data.fd) <= hfd)
				manager.add(fd);
			else if (events[i].events == EPOLLIN)
				manager.run(fd);
			else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
				manager.end(fd);
		}
		manager.out();
	}
	return (this->_status);
}

Application&
Application::operator=(const Application &rhs)
{
	Abortable::operator=(rhs);
	return (*this);
}
