/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 11:32:51 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Server.hpp"

Server::Server() :
	maxbdy(1048576),
	socket(-1)
{
	this->listen.first = "0.0.0.0";
	this->listen.second = "80";
	this->snames.push_back("*");
	this->isDuplicate["errors"] = false;
	this->isDuplicate["listen"] = false;
	this->isDuplicate["maxbdy"] = false;
	this->isDuplicate["snames"] = false;
}

Server::Server(const Server &src)
{
	*this = src;
}

Server::~Server()
{
	Logger::logs("Closing server: " + this->listen.first + ":" + this->listen.second + ".");
	Vector::free(this->routes);
	this->routes.clear();
	::close(this->socket);
}

void
Server::run()
{
	this->socket = Socket(this->listen);
	for (size_t i = 0, j = this->routes.size() - 1; i < j; i++)
	{
		if (i < j && this->routes[i]->target == "/*")
		{
			this->routes.push_back(this->routes[i]);
			this->routes.erase(this->routes.begin() + i--);
		}
	}
}

void
Server::addDirective(const int lineNumber, const string &directive)
{
	string key, value;
	stringstream ss(directive);

	ss >> key;
	if (!getline(ss, value)) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Failed to read server directive. skipping ...");
		return;
	}
	value = String::strim(value, " \f\r\t\v");

	if (key == "errors_page") {
		handleErrorsPage(lineNumber, value);
	} else if (key == "listen") {
		handleListen(lineNumber, value);
	} else if (key == "max_body_size") {
		handleMaxBodySize(lineNumber, value);
	} else if (key == "server_name") {
		handleServerName(lineNumber, value);
	} else {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Unknown server directive : " + directive);
	}
}

void
Server::PrintServer() const
{
	cout << "Errors: " << endl;
	for (map<int, string>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
		cout << "Error " << it->first << ": " << it->second << endl;
	}
	cout << "Listen: " << listen.first << ":" << listen.second << endl;
	cout << "Max Body Size: " << maxbdy << endl;
	cout << "Routes: " << endl;
	for (size_t i = 0; i < routes.size(); ++i) {
		routes[i]->printRoute();
	}
	cout << "Server Names: ";
	for (size_t i = 0; i < snames.size(); ++i) {
		cout << snames[i];
		if (i < snames.size() - 1) cout << ", ";
	}
	cout << endl;
	cout << "Socket: " << socket << endl;
	cout << "Target: " << target.first << ":" << target.second << endl;
}

size_t
Server::check(Request &request) const
{
	if (request.getLength() > this->maxbdy)
		return (413);
	return (200);
}

bool
Server::match(Request &request) const
{
	sockaddr_in	addr;
	socklen_t	len = sizeof(sockaddr_in);

    if (::getsockname(request.getSocket(), (sockaddr *) &addr, &len) == -1)
		return (Logger::fail("Failed to get socket information"), false);
	if ((this->target.first == 0 || this->target.first == addr.sin_addr.s_addr) && this->target.second == addr.sin_port)
	{
		const int		i = this->snames.size();
		const string	host(request.getHeader("host", ""));

		for (int j = 0; j < i; j++)
		{
			if (String::match(this->snames[j], host))
				return (true);
		}
	}
	return (false);
}

Server&
Server::operator=(const Server &rhs)
{
	this->errors = rhs.errors;
	this->listen = rhs.listen;
	this->maxbdy = rhs.maxbdy;
	this->routes = rhs.routes;
	this->snames = rhs.snames;
	this->socket = rhs.socket;
	this->target = rhs.target;
	this->isDuplicate = rhs.isDuplicate;
	return (*this);
}

void
Server::handleErrorsPage(const int lineNumber, const string &value)
{
	if (isDuplicate["errors"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Errors page directive already defined. New value(s) may be added ...");
	}
	isDuplicate["errors"] = true;
	stringstream 	ss(value);
	vector<int> 	codes;
	string			token;
	string 			page;
	vector<string>	tokens;

	while (ss >> token) {
		tokens.push_back(token);
	}
	if (tokens.size() < 2) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid error page parameter. Skipping ...");
		return;
	}
	page = tokens.back();	// last one contains the page
	tokens.pop_back();
	for (size_t i = 0; i < tokens.size(); ++i) {
		int code = ::atoi(tokens[i].c_str());
		if (code < 100 || code > 599) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Invalid error code: " + String::tostr(code) + ". Skipping ...");
			continue;
		}
		if (this->errors.find(code) != this->errors.end()) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Error code " + String::tostr(code) + " already set. Replacing old value ...");
		}
		this->errors[code] = page;
	}
}

void
Server::handleListen(const int lineNumber, const string &value)
{
	if (isDuplicate["listen"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Listen directive already defined. Replacing old value ...");
	}
	isDuplicate["listen"] = true;
	stringstream ss(value);
	string firstValue;
	ss >> firstValue;
	size_t pos1 = firstValue.find(':');
	if (pos1 != string::npos)
	{
		if (!firstValue.substr(0, pos1).empty()) {
			this->listen.first = firstValue.substr(0, pos1);
		}
		if (!firstValue.substr(pos1 + 1).empty()) {
			this->listen.second = firstValue.substr(pos1 + 1);
		}
	}
	else
	{
		size_t pos2 = firstValue.find('.');
		if (pos2 != string::npos) {
			this->listen.first = firstValue;
		}
		else {
			this->listen.second = firstValue;
		}
	}
	string extraParam;
	while (ss >> extraParam) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Skipping extra parameter: " + extraParam);
	}
}

void
Server::handleMaxBodySize(const int lineNumber, const string &value)
{
	if (isDuplicate["maxbdy"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Max body size directive already defined. Replacing old value ...");
	}
	if (value.empty())
	{
		Logger::warn("Line: " + String::tostr(lineNumber) + ". No value set for maxbdy. Skipping ...");
		return;
	}
	if (value[0] == '-')
	{
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Negative value is not possible for maxbdy. Skipping ...");
		return;
	}
	stringstream ss(value);
	size_t maxbdyValue = 0;
	ss >> maxbdyValue;
	if (ss.fail())
	{
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Failed to convert maxbdy value. Skipping ...");
		return;
	}
	if (!ss.eof()) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Too many arguments, first value used to set maxbdy.");
	}
	this->maxbdy =  maxbdyValue;
}

bool
Server::contains(const vector<string>& vec, const string& value) {
	for (size_t i = 0; i < vec.size(); ++i) {
		if (vec[i] == value) {
			return (true);
		}
	}
	return (false);
}

void
Server::handleServerName(const int lineNumber, const string &value)
{
	if (isDuplicate["snames"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Server name directive already defined. Adding new value(s) ...");
	}
	if (!isDuplicate["snames"]) {
		this->snames.clear();
	}
	stringstream ss(value);
	string name;
	while (ss >> name)
	{
		if (name == "*" && this->snames.size() == 1) {
			this->snames.push_back(name);
		}
		else if (!contains(snames,name)) {
			this->snames.push_back(name);
		}
		else {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Duplicate server name detected: " + name + ". Skipping ...");
		}
	}
}
