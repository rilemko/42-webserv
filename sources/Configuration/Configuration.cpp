/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 20:16:33 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 17:52:28 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration/Configuration.hpp"

Configuration::Configuration(const string &config, const int &epollfd) : _isNotRoute(false)
{
	ifstream				stream(config.c_str());
	size_t					context = 0, y = 0; // "context": 0 = none, 1 = server, 2 = route; "y": number of the line, can be used for error message if needed
	string					line;
	Server*					currentServer = NULL;
	Route*					currentRoute = NULL;
	ostringstream			buffer;
	ServerConfiguration*	serverConfig = NULL;
	RouteConfiguration*		routeConfig = NULL;

	if (!stream.is_open())
		this->abort("Failed to open the configuration file: " + config); // Aborting (set "_status" to false and print message to stderr)
	else
	{
		Logger::info("Reading configuration: " + config);
		while (getline(stream, line)) {
			line = String::strim(line, " \f\r\t\v");
			y++;
			string reformattedLine = _reformatLine(line, y);
			buffer << reformattedLine << endl;
		}
		istringstream stream2(buffer.str());
		while (this->_status && getline(stream2, line, '\n')) // While "_status" == true and there is a line
		{
			stringstream lineStream(line);
			lineStream >> y;
			getline(lineStream, line);
			line = String::strim(line, " \f\r\t\v"); // Trim the line
			if (context == 0 && line == "{") // If context = none and line == "{"
			{
				// Open server.
				Logger::dump("Entering server #" + String::tostr(this->_servers.size() + 1));
				currentServer = new Server;
				serverConfig = new ServerConfiguration();
				++context;
			}
			else if (context == 1 && String::match("location ** {", line)) // If context == server and line match the pattern "location ** {"
			{
				// Open route.
				Logger::dump("Open route: " + line);
				currentRoute  = new Route;
				size_t	pos = line.find(' ');
				currentRoute->target = line.substr(pos + 1, line.find(' ', pos + 1) - pos - 1);
				if (currentRoute->target.size() > 1)
					currentRoute->target = String::rtrim(currentRoute->target, "/"); // NEW: TRIM RIGHT SLASHES
				if (currentRoute->target == "/")
					currentRoute->target = "*";
				if (currentRoute->target[0] != '/')
					currentRoute->target = '/' + currentRoute->target; // NEW : ENSURE LEADING SLASH
				routeConfig = new RouteConfiguration();
				++context;
			}
			else if (line == "{") {
				if (context == 1) {
					Logger::warn("Line: " + String::tostr(y) + ". Wrong definition of route, missing 'location <target>'. Skipping ...");
					_isNotRoute = true;
				}
				if (context == 2) {
					Logger::warn("Line: " + String::tostr(y) + ". Too many open braces, beginning of invalid directives ...");
				}
				++context;
			}
			else if (context == 2 && line == "}") // If context == location and line == "}"
			{
				// Close route.
				if (_isNotRoute) {
					Logger::warn("Line: " + String::tostr(y) + ". End of incorrect route definition.");
					_isNotRoute = false;
				}
				else {
					Logger::dump("Closing route: " + line);
					currentServer->routes.push_back(currentRoute);
					currentRoute = NULL;
					delete routeConfig;
					routeConfig = NULL;
				}
				--context;
			}
			else if (context == 1 && line == "}") // If context == server and line == "}"
			{
				// Close server.
				Logger::dump("Closing server #" + String::tostr(this->_servers.size() + 1));
				this->_servers.push_back(currentServer);
				currentServer = NULL;
				delete serverConfig;
				serverConfig = NULL;
				--context;
			}
			else if (context > 2 && line == "}")
			{
				if (context == 3) {
					Logger::warn("Line: " + String::tostr(y) + ". End of invalid directives due to incorrect brace level ...");
				}
				--context;
			}
			else if (context <= 0 && line == "}")
			{
				this->abort("Line: " + String::tostr(y) + ". Closing an unopened brace ...");
			}
			else if (line.size()) // Otherwise, and if line is not empty
			{
				if (context == 1)
				{
					// Server directive.
					Logger::dump("Adding server directive: " + line);
					serverConfig->addDirective(*currentServer, y, line);
				}
				else if (context == 2 && !_isNotRoute)
				{
					// Location directive.
					Logger::dump("Adding route directive: " + line);
					routeConfig->addDirective(*currentRoute, y, line);
				}
				else if (_isNotRoute) {
					Logger::warn("Line: " + String::tostr(y) + ". Invalid route definition. Skipping ...");
				}
				else {
					Logger::warn("Line: " + String::tostr(y) + ". Invalid brace level. Skipping ...");
				}
			}
			else if (!line.size()) {
				// skip empty line
			}
			else {
				Logger::warn("Line: " + String::tostr(y) + ". Unrecognized directive: " + line + ". Skipping ...");
			}
		}
		if (context != 0) {
			Logger::warn("Unmatched braces. Please check the config file to avoid unexpected behaviour ...");
		}
		stream.close();

		// TEMP
		Logger::dump("Printing config after parsing");
		printConfig();

		// TEMP: Add one listener manually to 0.0.0.0:3000, must be created with parsing
//		this->_servers.push_back(new Server());
//		this->_servers.back()->listen.first = "0.0.0.0";
//		this->_servers.back()->listen.second = "3000";
//		this->_servers.back()->routes.push_back(new Route()); // -------------------
//		this->_servers.back()->routes.back()->target = "*";
//		this->_servers.back()->routes.back()->rooting = "www///";
//		this->_servers.back()->routes.back()->method.clear();
//		this->_servers.back()->routes.back()->method.push_back("GET");
//		this->_servers.back()->routes.back()->method.push_back("POST");
//		this->_servers.back()->errors[404] = "www/error404.html";
//		this->_servers.back()->routes.push_back(new Route()); // -------------------
//		this->_servers.back()->routes.back()->target = "/get";
//		this->_servers.back()->routes.back()->method.clear();
//		this->_servers.back()->routes.back()->method.push_back("GET");
//		this->_servers.back()->routes.push_back(new Route()); // -------------------
//		this->_servers.back()->routes.back()->target = "/post";
//		this->_servers.back()->routes.back()->method.clear();
//		this->_servers.back()->routes.back()->method.push_back("POST");
//		this->_servers.back()->routes.push_back(new Route()); // -------------------
//		this->_servers.back()->routes.back()->target = "/r";
//		this->_servers.back()->routes.back()->method.clear();
//		this->_servers.back()->routes.back()->method.push_back("GET");
//		this->_servers.back()->routes.back()->rewrite.first = 307;
//		this->_servers.back()->routes.back()->rewrite.second = "https://google.com";
//		this->_servers.back()->routes.push_back(new Route()); // -------------------
//		this->_servers.back()->routes.back()->rooting = "www///";
//		this->_servers.back()->routes.back()->target = "/sub";
//		this->_servers.back()->routes.back()->dindex = "index.html";
//		this->_servers.back()->routes.back()->method.clear();
//		this->_servers.back()->routes.back()->method.push_back("GET");

		// TEMP: Add a second listener manually to 0.0.0.0:3001, must be created with parsing
//		this->_servers.push_back(new Server());
//		this->_servers.back()->listen.first = "192.168.1.108";
//		this->_servers.back()->listen.second = "3001";

		// ==============================================
		// Add all servers to epoll
		// ==============================================

		epoll_event	e = {EPOLLIN | EPOLLET, {0}};

		for (size_t i = 0; i < this->_servers.size(); i++) // For each servers created by parsing ...
		{
			try
			{
				this->_servers[i]->run(); // Open the server socket

				sockaddr_in	addr;
				socklen_t	len = sizeof(sockaddr_in);

				if (getsockname(this->_servers[i]->socket, (sockaddr *) &addr, &len) == -1)
				{
					Logger::fail("Failed to add socket to epoll");
					delete this->_servers[i];
					this->_servers.erase(this->_servers.begin() + i--);
					continue;
				}

				this->_servers[i]->target.first = addr.sin_addr.s_addr;
				this->_servers[i]->target.second = addr.sin_port;

				Logger::info("Server listening on: " + this->_servers[i]->listen.first + ":" + this->_servers[i]->listen.second + ".");

				e.data.fd = this->_servers[i]->socket;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, e.data.fd, &e) == -1) // Add server socket to epoll
				{
					// Remove from vector<Server*> if fail
					Logger::fail("Failed to add socket to epoll");
					delete this->_servers[i];
					this->_servers.erase(this->_servers.begin() + i--);
				}
			}
			catch(const Socket::Exception &e) // Handle Socket creation exception
			{
				// Remove from vector<Server*> if fail
				Logger::fail(e.what());
				delete this->_servers[i];
				this->_servers.erase(this->_servers.begin() + i--);
			}
		}
	}
}

Configuration::Configuration(const Configuration &src) :
	Abortable(src)
{
	*this = src;
}

Configuration::~Configuration()
{
}

void
Configuration::printConfig(void) const
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		cout << "Server " << i + 1 << ":" << endl;
		this->_servers[i]->PrintServer();
		cout << endl;
	}
}

Configuration&
Configuration::operator=(const Configuration &rhs)
{
	Abortable::operator=(rhs);
	this->_servers = rhs._servers;
	return (*this);
}

Configuration::operator vector<Server*>() const
{
	return (this->_servers);
}

string
Configuration::_reformatLine(const string &line, int sourceLineNumber) {
	stringstream result;
	size_t len = line.size();
	bool newLine = true;

	for (size_t i = 0; i < len; ++i) {
		if (newLine) {
			result << sourceLineNumber << " ";
			newLine = false;
		}
		if (line.substr(i, 9) == "location " && i + 9 < len) {
			result << "location ";
			i += 9;
			while (i < len && line[i] != '{') {
				result << line[i];
				++i;
			}
			if (i < len && line[i] == '{') {
				result << " {\n" << sourceLineNumber << " ";
				newLine = false; //true;
			}
		} else {
			if (line[i] == '{' || line[i] == '}') {
				if (!newLine) {
					result << '\n';
				}
				result << sourceLineNumber << ' ' << line[i] << '\n';
				newLine = true;
			} else {
				result << line[i];
			}
		}
	}
	string reformatted = result.str();
	return reformatted;
}
