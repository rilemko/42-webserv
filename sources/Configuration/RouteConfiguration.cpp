/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfiguration.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/16 08:36:27 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteConfiguration.hpp"
#include "Server/Route.hpp"

map<string, bool> RouteConfiguration::_isDuplicate;
bool RouteConfiguration::_isDuplicateInitialized = false;

void
RouteConfiguration::addDirective(Route &route, const int lineNumber, const string &directive)
{
	initIsDuplicate();
	string key, value;
	stringstream ss(directive);

	ss >> key;
	if (!getline(ss, value)) {
		Logger::warn("Failed to read route directive. Skipping ...");
		return;
	}
	value = String::strim(value, " \f\r\t\v");

	if (key == "cgi_pass") {
		handleCgiPass(route, lineNumber, value);
	} else if (key == "listing") {
		handleListing(route, lineNumber, value);
	} else if (key == "index") {
		handleIndex(route, lineNumber, value);
	} else if (key == "methods") {
		handleMethods(route, lineNumber, value);
	} else if (key == "rewrite") {
		handleRewrite(route, lineNumber, value);
	} else if (key == "root") {
		handleRoot(route, lineNumber, value);
	} else if (key == "alias") {
		handleAlias(route, lineNumber, value);
	} else if (key == "upload_to") {
		handleUploadTo(route, lineNumber, value);
	} else {
		Logger::warn("Unknown route directive ; " + directive);
	}
}

void
RouteConfiguration::resetInitialization()
{
	_isDuplicateInitialized = false;
}

RouteConfiguration::RouteConfiguration()
{
}

RouteConfiguration::~RouteConfiguration()
{
}

void
RouteConfiguration::initIsDuplicate()
{
	if (!_isDuplicateInitialized) {
		_isDuplicate["cgi_pass"] = false;
		_isDuplicate["listing"] = false;
		_isDuplicate["index"] = false;
		_isDuplicate["methods"] = false;
		_isDuplicate["rewrite"] = false;
		_isDuplicate["root"] = false;
		_isDuplicate["alias"] = false;
		_isDuplicate["uploadTo"] = false;
		_isDuplicateInitialized = true;
	}
}

void
RouteConfiguration::handleCgiPass(Route &route, const int lineNumber, const string &value)
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
RouteConfiguration::handleListing(Route &route, const int lineNumber, const string &value)
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
RouteConfiguration::handleIndex(Route &route, const int lineNumber, const string &value)
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
RouteConfiguration::handleMethods(Route &route, const int lineNumber, const string &value)
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
RouteConfiguration::handleRewrite(Route &route, const int lineNumber, const string &value)
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
RouteConfiguration::handleAlias(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["alias"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Alias directive already set. Replacing old value...");
	} else {
		_isDuplicate["alias"] = true;
	}
	route.alias = Filesystem::normalize(String::rtrim(value, "/"));
	if (route.alias.empty()) {
		route.alias = "/";
	} else {
		if (!route.alias.empty() && route.alias[0] != '/') {
			route.alias = "/" + route.alias;
		}
		if (!route.alias.empty() && route.alias[route.alias.size() - 1] != '/') {
			route.alias = route.alias + "/";
		}
	}
}

void
RouteConfiguration::handleRoot(Route &route, const int lineNumber, const string &value)
{
	if (_isDuplicate["root"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Root directive already set. Replacing old value...");
	} else {
		_isDuplicate["root"] = true;
	}
	route.rooting = Filesystem::normalize(String::rtrim(value, "/"));
	if (route.rooting.empty()) {
		route.rooting = "/";
	}
	else
		route.rooting += "/";
}

void
RouteConfiguration::handleUploadTo(Route &route, const int lineNumber, const string &value)
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
