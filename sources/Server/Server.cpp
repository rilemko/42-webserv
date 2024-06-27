/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/27 22:32:05 by mconreau         ###   ########.fr       */
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
		if (i < j && this->routes[i]->target == "*")
		{
			this->routes.push_back(this->routes[i]);
			this->routes.erase(this->routes.begin() + i--);
		}
	}
}

void
Server::addDirective(const string &directive)
{
	string key, value;
	stringstream ss(directive);

	ss >> key;
	if (!getline(ss, value)) {
		Logger::warn("Failed to read server directive. skipping...");
		return;
	}
	value = String::strim(value, " \f\r\t\v");

	if (key == "errors_page") {
		handleErrorsPage(value);
	} else if (key == "listen") {
		handleListen(value);
	} else if (key == "max_body_size") {
		handleMaxBodySize(value);
	} else if (key == "server_name") {
		handleServerName(value);
	} else {
		Logger::warn("Unknown server directive : " + directive);
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
	if (this != &rhs) {
		this->errors = rhs.errors;
		this->listen = rhs.listen;
		this->maxbdy = rhs.maxbdy;
		this->routes = rhs.routes;
		this->snames = rhs.snames;
		this->socket = rhs.socket;
		this->target = rhs.target;
	}
	return (*this);
}

void
Server::handleErrorsPage(const string &value)
{
	stringstream 	ss(value);
	vector<int> 	codes;
	string			token;
	string 			page;
	vector<string>	tokens;

	while (ss >> token) {
		tokens.push_back(token);
	}
	if (tokens.size() < 2) {
		Logger::warn("Invalid error page parameter. Skipping...");
		return;
	}
	page = tokens.back();	// last one contains the page
	tokens.pop_back();
	for (size_t i = 0; i < tokens.size(); ++i) {
		int code = ::atoi(tokens[i].c_str());
		if (code < 100 || code > 599) {
			Logger::warn("Invalid error code. Skipping...");
			continue;
		}
		if (this->errors.find(code) == this->errors.end()) {
			this->errors[code] = page;
		}
		else {
			Logger::warn("Error code already in database. Skipping...");
		}
	}
}

void
Server::handleListen(const string &value)
{
	size_t pos = value.find(':');
	if (pos != string::npos)
	{
		this->listen.first = value.substr(0, pos);
		this->listen.second = value.substr(pos + 1);
	}
	else
	{
		this->listen.first = "0.0.0.0";
		this->listen.second = value;
	}
}

void
Server::handleMaxBodySize(const string &value)
{
	stringstream ss(value);
	size_t	maxbdyValue;
	ss >> maxbdyValue;
	if (ss.fail() || !ss.eof())
	{
		Logger::warn("Failed to convert maxbdy value, default value used.");
	}
	else
	{
		this->maxbdy = maxbdyValue;
	}
}

void
Server::handleServerName(const string &value)
{
	if (value.empty())
	{
		return;
	}
	this->snames.clear();
	stringstream ss(value);
	string name;
	while (ss >> name)
	{
		this->snames.push_back(name);
	}
}
