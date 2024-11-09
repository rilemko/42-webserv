/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/16 08:34:10 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfiguration.hpp"
#include "Server/Server.hpp"

map<string, bool> ServerConfiguration::_isDuplicate;
bool ServerConfiguration::_isDuplicateInitialized = false;

void
ServerConfiguration::addDirective(Server &server, const int lineNumber, const string &directive)
{
	initIsDuplicate();
	string key, value;
	stringstream ss(directive);

	ss >> key;
	if (!getline(ss, value)) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Failed to read server directive. skipping ...");
		return;
	}
	value = String::strim(value, " \f\r\t\v");

	if (key == "errors_page") {
		handleErrorsPage(server, lineNumber, value);
	} else if (key == "listen") {
		handleListen(server, lineNumber, value);
	} else if (key == "max_body_size") {
		handleMaxBodySize(server, lineNumber, value);
	} else if (key == "server_name") {
		handleServerName(server, lineNumber, value);
	} else {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Unknown server directive : " + directive);
	}
}

void
ServerConfiguration::resetInitialization()
{
	_isDuplicateInitialized = false;
}

ServerConfiguration::ServerConfiguration()
{
}

ServerConfiguration::~ServerConfiguration()
{
}

void
ServerConfiguration::initIsDuplicate()
{
	if (!_isDuplicateInitialized) {
		_isDuplicate["errors"] = false;
		_isDuplicate["listen"] = false;
		_isDuplicate["maxbdy"] = false;
		_isDuplicate["snames"] = false;
		_isDuplicateInitialized = true;
	}
}

bool
ServerConfiguration::contains(const vector<string>& vec, const string& value) {
	for (size_t i = 0; i < vec.size(); ++i) {
		if (vec[i] == value) {
			return (true);
		}
	}
	return (false);
}

void
ServerConfiguration::handleErrorsPage(Server &server, const int lineNumber, const string &value)
{
	if (_isDuplicate["errors"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Errors page directive already defined. New value(s) may be added ...");
	}
	_isDuplicate["errors"] = true;
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
		if (server.errors.find(code) != server.errors.end()) {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Error code " + String::tostr(code) + " already set. Replacing old value ...");
		}
		server.errors[code] = page;
	}
}

void
ServerConfiguration::handleListen(Server &server, const int lineNumber, const string &value)
{
	if (_isDuplicate["listen"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Listen directive already defined. Replacing old value ...");
	}
	_isDuplicate["listen"] = true;
	stringstream ss(value);
	string firstValue;
	ss >> firstValue;
	size_t pos1 = firstValue.find(':');
	if (pos1 != string::npos)
	{
		if (!firstValue.substr(0, pos1).empty()) {
			server.listen.first = firstValue.substr(0, pos1);
		}
		if (!firstValue.substr(pos1 + 1).empty()) {
			server.listen.second = firstValue.substr(pos1 + 1);
		}
	}
	else
	{
		size_t pos2 = firstValue.find('.');
		if (pos2 != string::npos) {
			server.listen.first = firstValue;
		}
		else {
			server.listen.second = firstValue;
		}
	}
	string extraParam;
	while (ss >> extraParam) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Skipping extra parameter: " + extraParam);
	}
}

void
ServerConfiguration::handleMaxBodySize(Server &server, const int lineNumber, const string &value)
{
	if (_isDuplicate["maxbdy"]) {
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
	if (maxbdyValue == 0) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". maxbdy value can't be zero. Skipping ...");
	}
	else
	{
		server.maxbdy =  maxbdyValue;
	}
}

void
ServerConfiguration::handleServerName(Server &server, const int lineNumber, const string &value)
{
	if (_isDuplicate["snames"]) {
		Logger::warn("Line: " + String::tostr(lineNumber) + ". Server name directive already defined. Adding new value(s) ...");
	}
	if (!_isDuplicate["snames"]) {
		server.snames.clear();
	}
	stringstream ss(value);
	string name;
	while (ss >> name)
	{
		if (name == "*" && server.snames.size() == 1) {
			server.snames.push_back(name);
		}
		else if (!contains(server.snames, name)) {
			server.snames.push_back(name);
		}
		else {
			Logger::warn("Line: " + String::tostr(lineNumber) + ". Duplicate server name detected: " + name + ". Skipping ...");
		}
	}
}
