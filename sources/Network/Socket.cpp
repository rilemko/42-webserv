/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/12 16:22:10 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Network/Socket.hpp"


Socket::Socket(const pair<string,string> &target, const int &type) :
	_afinet(String::match("*.*.*.*", target.first) ? AF_INET : (String::match("*:*:*:*:*:*:*:*", target.first) ? AF_INET6 : -1)),
	_socket(-1),
	_target(target)
{
	const int	opt = 1;

	if (this->_afinet  == -1)
		throw Exception(*this, "Wrong ip address format: " + target.first + ":" + target.second);
	if ((this->_socket = ::socket(this->_afinet, type, IPPROTO_TCP)) == -1)
		throw Exception(*this, "Failed to create socket for " + target.first + ":" + target.second);
	if (::setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		throw Exception(*this, "Failed to set option to socket for " + target.first + ":" + target.second);

	addrinfo	h = {}, *r;

	h.ai_family = this->_afinet;
	h.ai_flags = AI_NUMERICHOST | AI_PASSIVE;
	h.ai_socktype = SOCK_STREAM;
	h.ai_protocol = IPPROTO_TCP;

	if (::getaddrinfo(this->_target.first.c_str(), this->_target.second.c_str(), &h, &r) != 0)
		throw Exception(*this, "Failed to get addrinfo for " + this->_target.first + ":" + this->_target.second);
	else
	{
		if (::bind(this->_socket, r->ai_addr, r->ai_addrlen) == -1)
		{
			::freeaddrinfo(r);
			throw Exception(*this, "Failed to bind socket on " + this->_target.first + ":" + this->_target.second);
		}
		::freeaddrinfo(r);
	}

	if (::listen(this->_socket, SOMAXCONN) == -1)
		throw Exception(*this, "Failed to listen socket on " + this->_target.first + ":" + this->_target.second);
}

Socket::Socket(const Socket &src)
{
	*this = src;
}

Socket::~Socket()
{
}

void
Socket::close()
{
	::close(this->_socket);
}

Socket&
Socket::operator=(const Socket &rhs)
{
	this->_afinet = rhs._afinet;
	this->_socket = rhs._socket;
	return (*this);
}

Socket::operator int() const
{
	return (this->_socket);
}

Socket::Exception::Exception(Socket &socket, const string &message) :
	runtime_error(message)
{
	socket.close();
}
