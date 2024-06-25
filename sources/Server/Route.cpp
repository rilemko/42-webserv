/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/25 08:59:55 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Route.hpp"

Route::Route() :
	passcgi(""),
	dirlst(true),
	dindex("index.html"),
	rewrite(std::make_pair(0, "")),
	rooting("www"),
    upload("")
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

Route::Route(const Route &src) :
	passcgi(src.passcgi),
	dirlst(src.dirlst),
	dindex(src.dindex),
	method(src.method),
	rewrite(src.rewrite),
	rooting(src.rooting),
	target(src.target),
	upload(src.upload)
{
}

Route::~Route()
{
}

void
Route::addDirective(const string &directive)
{
	string key, value;
	stringstream ss(directive);

	ss >> key;
	if (!getline(ss, value)) {
		Logger::warn("Failed to read route directive. Skipping...");
		return;
	}
	value = String::strim(value, " \f\r\t\v");

	if (key == "cgi_pass") {
		handleCgiPass(value);
	} else if (key == "listing") {
		handleListing(value);
	} else if (key == "index") {
		handleIndex(value);
	} else if (key == "methods") {
		handleMethods(value);
	} else if (key == "rewrite") {
		handleRewrite(value);
	} else if (key == "root") {
		handleRoot(value);
	} else if (key == "upload_to") {
		handleUploadTo(value);
	} else {
		Logger::warn("Unknown route directive ; " + directive);
	}
}

void
Route::printRoute(void) const {
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
	return (String::match(this->target, request.getTarget()));
}

Route&
Route::operator=(const Route &rhs)
{
	if (this != &rhs) {
		passcgi = rhs.passcgi;
		dirlst = rhs.dirlst;
		dindex = rhs.dindex;
		method = rhs.method;
		rewrite = rhs.rewrite;
		rooting = rhs.rooting;
		target = rhs.target;
		upload = rhs.upload;
	}
	return (*this);
}

void
Route::handleCgiPass(const string &value) {
	this->passcgi = value;
}

void
Route::handleListing(const string &value) {
	this->dirlst = (value == "on");
}

void
Route::handleIndex(const string &value) {
	this->dindex = value;
}

void
Route::handleMethods(const string &value) {
	if (value == "*" || value.empty()) {
		return;
	}
	stringstream ss(value);
	string method;
	this->method.clear();
	while (ss >> method) {
		this->method.push_back(method);
	}
}

void
Route::handleRewrite(const string &value) {
	std::stringstream ss(value);
	size_t code;
	std::string url;
	if (!(ss >> code >> url)) {
		Logger::warn("Rewrite directive invalid. Skipping...");
		return;
	}
	this->rewrite = std::make_pair(code, url);
}

void
Route::handleRoot(const string &value) {
	this->rooting = value;
}

void
Route::handleUploadTo(const string &value) {
	this->upload = value;
}
