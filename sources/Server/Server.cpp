/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 17:53:44 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Server.hpp"
#include "Configuration/ServerConfiguration.hpp"

Server::Server() :
	maxbdy(1048576),
	socket(-1)
{
	this->listen.first = "0.0.0.0";
	this->listen.second = "80";
	this->snames.push_back("*");
}

Server::Server(const Server &src)
{
	*this = src;
}

Server::~Server()
{
	Logger::logs("Closing server: " + this->listen.first + ":" + this->listen.second + ".");
	Vector::free(this->routes);
	this->routes.clear();
	::close(this->socket);
}

void
Server::run()
{
	this->socket = Socket(this->listen);
	for (size_t i = 0, j = this->routes.size() - 1; i < j; i++)
	{
		if (i < j && this->routes[i]->target == "/*")
		{
			this->routes.push_back(this->routes[i]);
			this->routes.erase(this->routes.begin() + i--);
		}
	}
}

void
Server::PrintServer() const
{
	cout << "Errors: " << endl;
	for (map<int, string>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
		cout << "Error " << it->first << ": " << it->second << endl;
	}
	cout << "Listen: " << listen.first << ":" << listen.second << endl;
	cout << "Max Body Size: " << maxbdy << endl;
	cout << "Routes: " << endl;
	for (size_t i = 0; i < routes.size(); ++i) {
		routes[i]->printRoute();
	}
	cout << "Server Names: ";
	for (size_t i = 0; i < snames.size(); ++i) {
		cout << snames[i];
		if (i < snames.size() - 1) cout << ", ";
	}
	cout << endl;
	cout << "Socket: " << socket << endl;
	cout << "Target: " << target.first << ":" << target.second << endl;
}

size_t
Server::check(Request &request) const
{
	if (request.getLength() > this->maxbdy)
		return (413);
	return (200);
}

bool
Server::match(Request &request) const
{
	sockaddr_in	addr;
	socklen_t	len = sizeof(sockaddr_in);

    if (::getsockname(request.getSocket(), (sockaddr *) &addr, &len) == -1)
		return (Logger::fail("Failed to get socket information"), false);
	if ((this->target.first == 0 || this->target.first == addr.sin_addr.s_addr) && this->target.second == addr.sin_port)
	{
		const int		i = this->snames.size();
		const string	host(request.getHeader("host", ""));

		for (int j = 0; j < i; j++)
		{
			if (String::match(this->snames[j], host))
				return (true);
		}
	}
	return (false);
}

Server&
Server::operator=(const Server &rhs)
{
	if (this != &rhs) {
		this->errors = rhs.errors;
		this->listen = rhs.listen;
		this->maxbdy = rhs.maxbdy;
		this->routes = rhs.routes;
		this->snames = rhs.snames;
		this->socket = rhs.socket;
		this->target = rhs.target;
	}
	return (*this);
}
