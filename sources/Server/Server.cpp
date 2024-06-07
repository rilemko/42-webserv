/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/12 18:38:04 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Server.hpp"

Server::Server() :
	socket(-1)
{
}

Server::Server(const Server &src)
{
	*this = src;
}

Server::~Server()
{
	Logger::logs("Closing server: " + this->target.first + ":" + this->target.second);
	::close(this->socket);
}

void
Server::run()
{
	this->socket = Socket(this->target);
}

bool
Server::match() const
{
	return (true);
}

Server&
Server::operator=(const Server &rhs)
{
	this->routes = rhs.routes;
	this->socket = rhs.socket;
	this->target = rhs.target;
	return (*this);
}
