/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/11 23:49:56 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Response.hpp"

Response::Response(const int &socket) :
	_header("HTTP/1.1 {1} {2}\r\nServer: webserv\r\n"),
	_packet("\r\n"),
	_socket(socket),
	_status(200)
{
	(void) this->_socket;
}

Response::Response(const Response &src)
{
	*this = src;
}

Response::~Response()
{
}

void
Response::send()
{
	static HttpStatus	HttpStatus;
	
	this->_header = String::replace(this->_header, "{1}", String::tostr(this->_status));
	this->_header = String::replace(this->_header, "{2}", HttpStatus.get(this->_status));

	this->addHeader("Content-Length", String::tostr(this->_packet.size() - 2));
	this->addHeader("Content-Type", "text/html");
	this->addHeader("Date", Datetime(Datetime::RFC7231));

	Filesystem::send(this->_socket, this->_header + this->_packet);
}

void
Response::addCookie(const string &key, const string &value, const time_t &age)
{
	this->_header += "Set-Cookie: " + key + "=" + value
		+ (age > 0 ? "; Expires=" + Datetime(Datetime::RFC7231, ::time(0) + age) + "; Max-Age=" + String::tostr(age) : "")
		+ "\r\n";
}

void
Response::addHeader(const string &key, const string &value)
{
	this->_header += key + ": " + value + "\r\n";
}

void
Response::addPacket(const string &packet)
{
	this->_packet += packet;
}

void
Response::setStatus(const size_t &status)
{
	this->_status = status;
}

Response&
Response::operator=(const Response &rhs)
{
	this->_header = rhs._header;
	this->_packet = rhs._packet;
	this->_socket = rhs._socket;
	return (*this);
}
