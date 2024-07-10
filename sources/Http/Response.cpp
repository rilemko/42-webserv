/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 10:16:44 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Response.hpp"

Response::Response(const int &socket) :
	_header("HTTP/1.1 %1 %2\r\nServer: Webserv/1.0.0\r\n"),
	_mimetp("text/html"),
	_packet("\r\n"),
	_socket(socket),
	_status(200)
{
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

	this->_header = String::replace(this->_header, "%1", String::tostr(this->_status));
	this->_header = String::replace(this->_header, "%2", HttpStatus.get(this->_status));

	this->addHeader("Content-Length", String::tostr(this->_packet.size() - 2));
	this->addHeader("Content-Type", this->_mimetp);
	this->addHeader("Date", Datetime(Datetime::RFC7231));

	Filesystem::send(this->_socket, this->_header + this->_packet);
}

void
Response::send(const string &packet)
{
	static HttpStatus	HttpStatus;
	
	this->_header = String::replace(this->_header, "%1", String::tostr(this->_status));
	this->_header = String::replace(this->_header, "%2", HttpStatus.get(this->_status));

	this->addHeader("Content-Length", String::tostr(packet.substr(packet.find("\r\n\r\n") + 4).size()));
	this->addHeader("Content-Type", this->_mimetp);
	this->addHeader("Date", Datetime(Datetime::RFC7231));

	Filesystem::send(this->_socket, this->_header + packet);
}

Response&
Response::addCookie(const string &key, const string &value, const time_t &age)
{
	this->_header += "Set-Cookie: " + key + "=" + value
		+ (age > 0 ? "; Expires=" + Datetime(Datetime::RFC7231, ::time(0) + age) + "; Max-Age=" + String::tostr(age) : "")
		+ "\r\n";
	return (*this);
}

Response&
Response::addHeader(const string &key, const string &value)
{
	this->_header += key + ": " + value + "\r\n";
	return (*this);
}

Response&
Response::addPacket(const string &packet)
{
	this->_packet += packet;
	return (*this);
}

Response&
Response::setMimeType(const string &type)
{
	this->_mimetp = type;
	return (*this);
}

Response&
Response::setSessid(const string &sessid)
{
	this->addCookie("sessid", sessid);
	return (*this);
}

Response&
Response::setStatus(const size_t &status)
{
	this->_status = status;
	return (*this);
}

Response&
Response::operator=(const Response &rhs)
{
	this->_header = rhs._header;
	this->_packet = rhs._packet;
	this->_sessid = rhs._sessid;
	this->_socket = rhs._socket;
	this->_status = rhs._status;
	return (*this);
}
