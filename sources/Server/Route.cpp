/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 17:54:41 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Route.hpp"
#include "Configuration/RouteConfiguration.hpp"

Route::Route() :
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

void
Route::printRoute(void) const
{
	cout << "Route Details:" << endl;
	cout << "  Pass CGI: " << passcgi << endl;
	cout << "  Directory Listing: " << (dirlst ? "on" : "off") << endl;
	cout << "  Default Index: " << dindex << endl;
	cout << "  Methods: ";
	for (size_t i = 0; i < method.size(); ++i) {
		cout << method[i];
		if (i < method.size() - 1) cout << ", ";
	}
	cout << endl;
	cout << "  Rewrite: " << rewrite.first << " -> " << rewrite.second << endl;
	cout << "  Alias: " << alias << endl;
	cout << "  Root: " << rooting << endl;
	cout << "  Target: " << target << endl;
	cout << "  Upload: " << upload << endl;
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
	if (this != &rhs) {
		this->dirlst = rhs.dirlst;
		this->dindex = rhs.dindex;
		this->method = rhs.method;
		this->passcgi = rhs.passcgi;
		this->rewrite = rhs.rewrite;
		this->rooting = rhs.rooting;
		this->target = rhs.target;
		this->upload = rhs.upload;
	}
	return (*this);
}
