/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/26 19:25:47 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Route.hpp"
#include "Configuration/RouteConfiguration.hpp"

Route::Route() :
	alias(""),
	dirlst(true),
	dindex("/index.html"),
	rooting("")
{
	this->method.push_back("GET");
	this->method.push_back("POST");
	this->method.push_back("HEAD");
	this->method.push_back("PUT");
	this->method.push_back("DELETE");
	this->method.push_back("CONNECT");
	this->method.push_back("OPTIONS");
	this->method.push_back("TRACE");
	this->method.push_back("PATCH");
}

Route::Route(const Route &src)
{
	*this = src;
}

Route::~Route()
{
}

size_t
Route::check(Request &request) const
{
	if (!Vector::find<string>(this->method, request.getMethod()))
		return (405);
	return (200);
}

bool
Route::match(Request &request) const
{
	return (String::match((this->target), request.getTarget()) || String::match((this->target + "/*"), request.getTarget()));
}

Route&
Route::operator=(const Route &rhs)
{
	this->dirlst = rhs.dirlst;
	this->dindex = rhs.dindex;
	this->method = rhs.method;
	this->passcgi = rhs.passcgi;
	this->rewrite = rhs.rewrite;
	this->rooting = rhs.rooting;
	this->target = rhs.target;
	this->upload = rhs.upload;
	return (*this);
}
