/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 20:16:33 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/02 14:48:09 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration/Configuration.hpp"

Configuration::Configuration(const string &config, const int &epollfd)
{
	ifstream	stream(config.c_str());
	size_t		context = 0, y = 0; // "context": 0 = none, 1 = server, 2 = route; "y": number of the line, can be used for error message if needed
	string		line;
	Server*		currentServer = NULL;
	Route*		currentRoute = NULL;

	if (!stream.is_open())
		this->abort("Failed to open the configuration file: " + config); // Aborting (set "_status" to false and print message to stderr)
	else
	{
		Logger::info("Reading configuration: " + config);
		while (this->_status && getline(stream, line, '\n') && ++y) // While "_status" == true and there is a line
		{
			line = String::strim(line, " \f\r\t\v"); // Trim the line
			if (context == 0 && line == "{") // If context = none and line == "{"
			{
				// Open server.
				Logger::dump("Entering server #" + String::tostr(this->_servers.size() + 1));
				currentServer = new Server;
				++context;
			}
			else if (context == 1 && String::match("location ** {", line)) // If context == server and line match the pattern "location ** {"
			{
				// Open route.
				Logger::dump("Open route: " + line);
				currentRoute  = new Route;
				size_t	pos = line.find(' ');
				currentRoute->target = line.substr(pos + 1, line.find(' ', pos + 1) - pos - 1);
				if (currentRoute->target == "/") {
					currentRoute->target = "*";
				}
				++context;
			}
			else if (context == 2 && line == "}") // If context == location and line == "}"
			{
				// Close route.
				Logger::dump("Close route: " + line);
				currentServer->routes.push_back(currentRoute);
				currentRoute = NULL;
				--context;
			}
			else if (context == 1 && line == "}") // If context == server and line == "}"
			{
				// Close server.
				Logger::dump("Closing server #" + String::tostr(this->_servers.size() + 1));
				this->_servers.push_back(currentServer);
				currentServer = NULL;
				--context;
			}
			else if (line.size()) // Otherwise, and if line is not empty
			{
				if (context == 1)
				{
					// Server directive.
					Logger::dump("Add server directive: " + line);
					currentServer->addDirective(y, line);
				}
				if (context == 2)
				{
					// Location directive.
					Logger::dump("Add route directive: " + line);
					currentRoute->addDirective(y, line);
				}
			}
			else if (!line.size()) {
				// skip empty line
			}
			else {
				Logger::warn("Line: " + String::tostr(y) + ". Unrecognized directive: " + line + ". Skipping ...");
			}
		}
		stream.close();

		// TEMP
		Logger::dump("Print config after parsing");
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

		epoll_event	e = {EPOLLIN, {0}};

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

				Logger::info("Server listen on: " + this->_servers[i]->listen.first + ":" + this->_servers[i]->listen.second + ".");

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
