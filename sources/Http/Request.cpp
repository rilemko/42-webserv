/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/28 22:46:29 by mconreau         ###   ########.fr       */
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
		return (this->unchunk(packet));
	}
	else
	{
		if (!String::match("?* ?* HTTP/*\r\n?*\r\n\r\n*", packet))
			return (this->_status = 400, (void) NULL);
		
		size_t			p = packet.find("\r\n\r\n");
		const string	h = packet.substr(0, p + 2);
		stringstream	t(h.substr(0, (p = h.find("\r\n") + 2) - 2));
		string			v;

		t >> this->_method >> this->_target >> v;

		if (v != "HTTP/1.1")
			return (this->_status = 505, (void) NULL);
		if (this->_target.size() > 2000)
			return (this->_status = 414, (void) NULL);

		size_t			d = 0;

		if ((d = this->_target.find('?')) != string::npos)
		{
			this->_params = this->_target.substr(d + 1);
			this->_target = this->_target.substr(0, d);
		}

		for (size_t e = 0; (e = h.find("\r\n", p)) != string::npos;)
		{
			string	lne(h.substr(p, e - p));
			string 	key(lne.substr(0, d = lne.find(':')));
			string	val(lne.substr(d + 1));

			if (val.size() > 8000)
				return (this->_status = 431, (void) NULL);
			this->_header[String::lowercase(String::strim(key, " "))] = String::strim(val, " \r\n");
			p = e + 2;
		}

		if (this->_header.find("transfer-encoding") != this->_header.end() && this->_header["transfer-encoding"] == "chunked")
		{
			this->unchunk(packet.substr(packet.find("\r\n\r\n") + 4));
			this->_status = 2;
		}
		else if (this->_method == "POST" || this->_method == "PUT" || this->_method == "PATCH")
		{
			stringstream	ss;

			ss << this->getHeader("content-length", String::tostr(18446744073709551615UL));
			ss >> this->_length;
			this->_packet = packet.substr(packet.find("\r\n\r\n") + 4, this->_length);
		}
	}
	/*
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

		if (this->_header.find("transfer-encoding") != this->_header.end() && this->_header["transfer-encoding"] == "chunked")
		{
			this->unchunk(packet.substr(packet.find("\r\n\r\n") + 4));
			this->_status = 2;
		}
		else if (this->_method == "POST" || this->_method == "PUT" || this->_method == "PATCH")
		{
			stringstream ss;
			ss << this->getHeader("content-length", "0");
			ss >> this->_length;

			this->_packet = packet.substr(packet.find("\r\n\r\n") + 4, this->_length);
		}
	}
	*/
}

void
Request::unchunk(const string &packet)
{
	unsigned int	l = 0;

	for (size_t	e = 0, i = 0, p = 0; (e = packet.find("\r\n", p)) != string::npos;)
	{
		if (!(i % 2))
		{
			const string	sl = packet.substr(p, e - p);

			if (e == p || sl.find_first_not_of("0123456789abcdef") != string::npos)
				return (this->_status = 400, (void) NULL);

			stringstream 	ss;
			ss << hex << sl;
			ss >> l;
			if (l == 0)
				return (this->_status = 200, (void) NULL);
		}
		else
		{
			this->_packet += packet.substr(p, l);
			this->_length += l;
		}
		p = e + 2;
		++i;
	}
	this->_status = 1;
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
