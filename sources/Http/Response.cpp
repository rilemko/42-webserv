/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/19 20:58:09 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Response.hpp"

Response::Response(const int &socket) :
	_packet("HTTP/1.1 {c} OK\r\n"),
	_socket(socket)
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
Response::send(const string &packet)
{

	this->_packet = String::replace(this->_packet, "{c}", "200");

	this->addHeader("Content-Length", String::tostr(packet.size()));
	this->addHeader("Content-Type", "text/html");
	this->addHeader("Date", Datetime(Datetime::RFC7231));

	this->_packet += "\r\n";
	this->_packet += packet;

	const int	len = this->_packet.size(), socket = this->_socket;
	int 		b = 0, r = len, total = 0;

	while (total < len)
	{
		r -= (b = ::send(socket, &this->_packet[total], r, MSG_DONTWAIT));
		total += b;
	}
}

void
Response::addCookie(const string &key, const string &value, const time_t &age)
{
	this->_packet += "Set-Cookie: " + key + "=" + value
		+ (age > 0 ? "; Expires=" + Datetime(Datetime::RFC7231, ::time(0) + age) + "; Max-Age=" + String::tostr(age) : "")
		+ "\r\n";
}

void
Response::addHeader(const string &key, const string &value)
{
	this->_packet += key + ": " + value + "\r\n";
}

Response&
Response::operator=(const Response &rhs)
{
	this->_packet = rhs._packet;
	this->_socket = rhs._socket;
	return (*this);
}
