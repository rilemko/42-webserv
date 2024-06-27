/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/27 22:28:07 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Request.hpp"

Request::Request(const int &socket) :
	_length(0),
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
	
	if (this->_status == 1 || this->_status == 2)
	{
		if (!String::match("?*\r\n*\r\n", packet))
			return (this->_status = 400, (void) NULL);

		unsigned int i;   
		stringstream ss;
		ss << hex << packet.substr(0, packet.find("\r\n"));
		ss >> i;

		if (i == 0)
			return (this->_status = 200, (void) NULL);
		else
		{
			this->_packet += packet.substr(packet.find("\r\n") + 2, i);
			//cout << ":" << packet << ":" << endl;
			this->_length += i;
			return (this->_status = 1, (void) NULL);
		}
	}
	
	size_t 			p = packet.find("\r\n\r\n");
	const string 	h(packet.substr(0, p));
	stringstream	stream(h.substr(0, (p = h.find("\r\n", 0) + 2) - 2));
	string			version;

	if (!(stream >> this->_method >> this->_target >> version))
		return (this->_status = 400, (void) NULL);
	else
	{
		if (version != "HTTP/1.1")
			return (this->_status = 505, (void) NULL);
		if (this->_target.size() > 2000)
			return (this->_status = 414, (void) NULL);

		size_t	m;
		if ((m = this->_target.find('?')) != string::npos)
		{
			this->_params = this->_target.substr(m + 1);
			this->_target = this->_target.substr(0, m);
		}

		size_t	e, f;
		while ((e = h.find("\r\n", p)) != string::npos)
		{
			string	lne(h.substr(p, e - p));
			string 	key(lne.substr(0, f = lne.find(':')));
			string	val(lne.substr(f + 1));

			if (val.size() > 8000)
				return (this->_status = 431, (void) NULL);
			this->_header[String::lowercase(String::strim(key, " "))] = String::strim(val, " \r\n");
			p = e + 2;
		}
		string	lne(h.substr(p, e - p));
		string 	key(lne.substr(0, f = lne.find(':')));
		string	val(lne.substr(f + 1));
		this->_header[String::lowercase(String::strim(key, " "))] = String::strim(val, " \r\n");

		stringstream ss;
		ss << this->getHeader("content-length", "0");
		ss >> this->_length;

		this->_packet = packet.substr(packet.find("\r\n\r\n") + 4, this->_length);

		if (this->_header.find("transfer-encoding") != this->_header.end() && this->_header["transfer-encoding"] == "chunked")
			this->_status = 2;
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

void
Request::setTarget(const string &target)
{
	this->_target = target;
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
	this->_status = rhs._status;
	this->_target = rhs._target;
	return (*this);
}
