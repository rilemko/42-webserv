/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 11:32:15 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Route.hpp"

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
	this->isDuplicate["cgi_pass"] = false;
	this->isDuplicate["listing"] = false;
	this->isDuplicate["index"] = false;
	this->isDuplicate["methods"] = false;
	this->isDuplicate["rewrite"] = false;
	this->isDuplicate["root"] = false;
	this->isDuplicate["alias"] = false;
	this->isDuplicate["uploadTo"] = false;
}

Route::Route(const Route &src)
{
	*this = src;
}

Route::~Route()
{
}

void
Route::addDirective(const int lineNumber, const string &directive)
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
		handleCgiPass(lineNumber, value);
	} else if (key == "listing") {
		handleListing(lineNumber, value);
	} else if (key == "index") {
		handleIndex(lineNumber, value);
	} else if (key == "methods") {
		handleMethods(lineNumber, value);
	} else if (key == "rewrite") {
		handleRewrite(lineNumber, value);
	} else if (key == "root") {
		handleRoot(lineNumber, value);
	} else if (key == "alias") {
		handleAlias(lineNumber, value);
	} else if (key == "upload_to") {
		handleUploadTo(lineNumber, value);
	} else {
		Logger::warn("Unknown route directive : " + directive);
	}
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
	this->alias = rhs.alias;
	this->dirlst = rhs.dirlst;
	this->dindex = rhs.dindex;
	this->method = rhs.method;
	this->passcgi = rhs.passcgi;
	this->rewrite = rhs.rewrite;
	this->rooting = rhs.rooting;
	this->target = rhs.target;
	this->upload = rhs.upload;
	this->isDuplicate = rhs.isDuplicate;
	return (*this);
}

void
Route::handleCgiPass(const int lineNumber, const string &value)
{
	if (isDuplicate["cgi_pass"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". CGI pass directive already set . Replacing old value ...");
	}
	else {
		isDuplicate["cgi_pass"] = true;
	}
	this->passcgi = value;
}

void
Route::handleListing(const int lineNumber, const string &value)
{
	if (value == "on" || value == "off") {
		if (isDuplicate["listing"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Listing directive already set. Replacing old value ...");
		} else {
			isDuplicate["listing"] = true;
		}
		this->dirlst = (value == "on");
	} else {
		if (isDuplicate["listing"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid listing parameter. Skipping duplicate directive ...");
		} else {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid listing parameter. Skipping ...");
			isDuplicate["listing"] = true;
		}
	}
}

void
Route::handleIndex(const int lineNumber, const string &value)
{
	if (isDuplicate["index"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Index directive already set. Replacing old value ...");
	} else {
		isDuplicate["index"] = true;
	}
	if (value[0] == '/') {
		this->dindex = value;
	}
	else {
		this->dindex = "/" + value;
	}
}

void
Route::handleMethods(const int lineNumber, const string &value)
{
	if (value == "*")
		return ;
	if (isDuplicate["methods"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Methods directive already set. Adding new value(s) ...");
	} else {
		this->method.clear();
		isDuplicate["methods"] = true;
	}
	stringstream ss(value);
	string method;
	while (ss >> method)
	{
		this->method.push_back(method);
	}
}

void
Route::handleRewrite(const int lineNumber, const string &value)
{
	stringstream ss(value);
	size_t code;
	string url;
	bool isValid = (ss >> code >> url) && (ss.eof());
	if (isValid) {
		if (isDuplicate["rewrite"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Rewrite directive already set. Replacing old value ...");
		} else {
			isDuplicate["rewrite"] = true;
		}
		this->rewrite = ::make_pair(code, url);
	} else {
		if (isDuplicate["rewrite"]) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid rewrite directive. Skipping duplicate ...");
		} else {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid rewrite directive. Skipping ...");
			isDuplicate["rewrite"] = true;
		}
	}
}

void
Route::handleAlias(const int lineNumber, const string &value)
{
	if (isDuplicate["alias"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Root directive already set. Replacing old value...");
	} else {
		isDuplicate["alias"] = true;
	}
	this->alias = (this->rooting.size() && value[0] != '/' ? "/" : "") + value; // NEW: ADD LEADING SLASH IF ROOT
}

void
Route::handleRoot(const int lineNumber, const string &value)
{
	if (isDuplicate["root"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Root directive already set. Replacing old value...");
	} else {
		isDuplicate["root"] = true;
	}
	this->rooting = value + (this->alias.size() && value[value.size() - 1] != '/' ? "/" : ""); // NEW: ADD TRAILING SLASH IF NO ALIAS
}

void
Route::handleUploadTo(const int lineNumber, const string &value)
{
	if (isDuplicate["uploadTo"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Upload_to directive already set. Replacing old value...");
	} else {
		isDuplicate["uploadTo"] = true;
	}
	this->upload = value;
	if (!upload.empty() && upload[upload.size() - 1] != '/') {
		this->upload += '/';
	}
}
