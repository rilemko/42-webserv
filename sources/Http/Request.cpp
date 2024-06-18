/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/17 21:02:55 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Request.hpp"

Request::Request(const int &socket) :
	_socket(socket),
	_status(200)
{
}

Request::Request(const Request &src)
{
	*this = src;
}

Request::~Request()
{
}

void
Request::recv()
{
	const string	packet(Filesystem::recv(this->_socket));

	size_t 			p = packet.find("\r\n\r\n");
	const string 	h(packet.substr(0, p));
	stringstream	stream(h.substr(0, (p = h.find("\r\n", 0) + 2) - 2));

	if (!(stream >> this->_method >> this->_target >> this->_version))
		return (this->_status = 400, (void) NULL);
	else
	{
		if (this->_version != "HTTP/1.1")
			return (this->_status = 505, (void) NULL);
		size_t	m;
		if ((m = this->_target.find('?')) != string::npos)
		{
			this->_params = this->_target.substr(m + 1);
			this->_target = this->_target.substr(0, m);
		}

		size_t	e, f;
		this->_packet = packet.substr(packet.find("\r\n\r\n") + 4);
		this->_length = this->_packet.size();
		
		while ((e = h.find("\r\n", p)) != string::npos)
		{
			string	lne(h.substr(p, e - p));
			string 	key(lne.substr(0, f = lne.find(':')));
			string	val(lne.substr(f + 1));

			this->_header[String::lowercase(String::strim(key, " "))] = String::strim(val, " \r\n");
			p = e + 2;
		}
	}
}

string
Request::getHeader(const string &key, const string &other)
{
	return (this->_header.find(key) != this->_header.end() ? this->_header[key] : other);
}

size_t
Request::getLength() const
{
	return (this->_length);
}

string
Request::getMethod() const
{
	return (this->_method);
}

string
Request::getPacket() const
{
	return (this->_packet);
}

string
Request::getParams() const
{
	return (this->_params);
}

int
Request::getSocket() const
{
	return (this->_socket);
}

size_t
Request::getStatus() const
{
	return (this->_status);
}

string
Request::getTarget() const
{
	return (this->_target);
}

Request&
Request::operator=(const Request &rhs)
{
	this->_header = rhs._header;
	this->_length = rhs._length;
	this->_method = rhs._method;
	this->_packet = rhs._packet;
	this->_params = rhs._params;
	this->_socket = rhs._socket;
	this->_target = rhs._target;
	return (*this);
}
