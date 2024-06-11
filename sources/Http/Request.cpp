/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/11 19:28:37 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Request.hpp"

#include <iostream>

Request::Request(const int &socket) :
	_socket(socket)
{
}

Request::Request(const Request &src) :
	Abortable(src),
	Mappable(src)
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

	if (!(stream >> this->_method >> this->_uri >> this->_version))
		this->abort();
	else
	{
		size_t	m;
		if ((m = this->_uri.find('?')) != string::npos)
		{
			this->_params = this->_uri.substr(m + 1);
			this->_uri = this->_uri.substr(0, m);
		}

		size_t	e, f;
		this->_body = packet.substr(p + 1);
		this->_size = this->_body.size();
		
		while ((e = h.find("\r\n", p)) != string::npos)
		{
			string	lne(h.substr(p, e - p));
			string 	key(lne.substr(0, f = lne.find(':')));
			string	val(lne.substr(f + 1));

			this->set(String::lowercase(String::strim(key, " \r\n")), String::lowercase(String::strim(val, " \r\n")));
			p = e + 2;
		}
	}
}

Request&
Request::operator=(const Request &rhs)
{
	Abortable::operator=(rhs);
	Mappable::operator=(rhs);
	this->_socket = rhs._socket;
	return (*this);
}
