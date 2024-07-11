/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfiguration.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/11 14:17:07 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteConfiguration.hpp"
#include "Server/Route.hpp"

RouteConfiguration::RouteConfiguration()
{
	this->_isDuplicate["cgi_pass"] = false;
	this->_isDuplicate["listing"] = false;
	this->_isDuplicate["index"] = false;
	this->_isDuplicate["methods"] = false;
	this->_isDuplicate["rewrite"] = false;
	this->_isDuplicate["root"] = false;
	this->_isDuplicate["alias"] = false;
	this->_isDuplicate["uploadTo"] = false;
}

RouteConfiguration::RouteConfiguration(const RouteConfiguration &src)
{
	*this = src;
}

RouteConfiguration::~RouteConfiguration()
{
}

RouteConfiguration&
RouteConfiguration::operator=(const RouteConfiguration &rhs)
{
	if (this != &rhs) {
		this->_isDuplicate = rhs._isDuplicate;
	}
	return (*this);
}

void
RouteConfiguration::addDirective(Route &route, const int lineNumber, const string &directive)
{
	string key, value;
	stringstream ss(directive);

	ss >> key;
	if (!getline(ss, value)) {
		Logger::warn("Failed to read route directive. Skipping ...");
		return;
	}
	value = String::strim(value, " \f\r\t\v");

	if (key == "cgi_pass") {
		_handleCgiPass(route, lineNumber, value);
	} else if (key == "listing") {
		_handleListing(route, lineNumber, value);
	} else if (key == "index") {
		_handleIndex(route, lineNumber, value);
	} else if (key == "methods") {
		_handleMethods(route, lineNumber, value);
	} else if (key == "rewrite") {
		_handleRewrite(route, lineNumber, value);
	} else if (key == "root") {
		_handleRoot(route, lineNumber, value);
	} else if (key == "alias") {
		_handleAlias(route, lineNumber, value);
	} else if (key == "upload_to") {
		_handleUploadTo(route, lineNumber, value);
	} else {
		Logger::warn("Unknown route directive ; " + directive);
	}
}

void
RouteConfiguration::_handleCgiPass(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["cgi_pass"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". CGI pass directive already set . Replacing old value ...");
	}
	else {
		_isDuplicate["cgi_pass"] = true;
	}
	route.passcgi = value;
}

void
RouteConfiguration::_handleListing(Route &route, const int lineNumber, const string &value)
{
	if (value == "on" || value == "off") {
		if (_isDuplicate["listing"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Listing directive already set. Replacing old value ...");
		} else {
			_isDuplicate["listing"] = true;
		}
		route.dirlst = (value == "on");
	} else {
		if (_isDuplicate["listing"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid listing parameter. Skipping duplicate directive ...");
		} else {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid listing parameter. Skipping ...");
			_isDuplicate["listing"] = true;
		}
	}
}

void
RouteConfiguration::_handleIndex(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["index"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Index directive already set. Replacing old value ...");
	} else {
		_isDuplicate["index"] = true;
	}
	if (value[0] == '/') {
		route.dindex = value;
	}
	else {
		route.dindex = "/" + value;
	}
}

void
RouteConfiguration::_handleMethods(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["methods"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Methods directive already set. Adding new value(s) ...");
	} else {
		route.method.clear();
		_isDuplicate["methods"] = true;
	}
	stringstream ss(value);
	string method;
	while (ss >> method)
	{
		if (method == "*") {
			route.method.clear();
			route.method.push_back("GET");
			route.method.push_back("POST");
			route.method.push_back("HEAD");
			route.method.push_back("PUT");
			route.method.push_back("DELETE");
			route.method.push_back("CONNECT");
			route.method.push_back("OPTIONS");
			route.method.push_back("TRACE");
			route.method.push_back("PATCH");
			return;
		} else {
			route.method.push_back(method);
		}
	}
}

void
RouteConfiguration::_handleRewrite(Route &route, const int lineNumber, const string &value)
{
	stringstream ss(value);
	size_t code;
	string url;
	bool isValid = (ss >> code >> url) && (ss.eof());
	if (isValid) {
		if (_isDuplicate["rewrite"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Rewrite directive already set. Replacing old value ...");
		} else {
			_isDuplicate["rewrite"] = true;
		}
		route.rewrite = ::make_pair(code, url);
	} else {
		if (_isDuplicate["rewrite"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid rewrite directive. Skipping duplicate ...");
		} else {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid rewrite directive. Skipping ...");
			_isDuplicate["rewrite"] = true;
		}
	}
}

void
RouteConfiguration::_handleAlias(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["alias"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Alias directive already set. Replacing old value...");
	} else {
		_isDuplicate["alias"] = true;
	}
	route.alias = (route.rooting.size() && value[0] != '/' ? "/" : "") + value; // NEW: ADD LEADING SLASH IF ROOT
}

void
RouteConfiguration::_handleRoot(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["root"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Root directive already set. Replacing old value...");
	} else {
		_isDuplicate["root"] = true;
	}
	route.rooting = value + (route.alias.size() && value[value.size() - 1] != '/' ? "/" : ""); // NEW: ADD TRAILING SLASH IF NO ALIAS
}

void
RouteConfiguration::_handleUploadTo(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["uploadTo"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Upload_to directive already set. Replacing old value...");
	} else {
		_isDuplicate["uploadTo"] = true;
	}
	route.upload = value;
	if (!route.upload.empty() && route.upload[route.upload.size() - 1] != '/') {
		route.upload += '/';
	}
}
