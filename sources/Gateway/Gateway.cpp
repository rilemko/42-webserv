/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muteza <muteza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:13 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/03 17:59:05 by muteza           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Gateway/Gateway.hpp"

Gateway::Gateway() {}

Gateway::Gateway(const Gateway &src) {
	*this = src;
}

Gateway::~Gateway() {}

Gateway& Gateway::operator=(const Gateway &rhs) {
	if (this != &rhs) {
		this->v = rhs.v;
	}
	return *this;
}

void Gateway::addenv(const std::string& key, const std::string& value) {
	std::string str = key + "=" + value;
	v.push_back(str);
}

char** Gateway::put_to_env() {
	char** envp = new char*[v.size() + 1];
	for (size_t i = 0; i < v.size(); ++i) {
		envp[i] = new char[v[i].size() + 1];
		String::custom_strcpy(envp[i], v[i].c_str());
	}
	envp[v.size()] = NULL;
	return envp;	
}

const char *Gateway::getAbsolutePathOfFile(const char *fileName) {
   static char absPath[PATH_MAX];

	DIR* dir = opendir(".");
	if (dir == NULL) {
		std::cerr << "Error in opendir: " << strerror(errno) << std::endl;
		return NULL;
	}

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG) { // Check if it's a regular file
			if (std::strcmp(entry->d_name, fileName) == 0) {
				String::custom_strcpy(absPath, "./");
				std::strcat(absPath, entry->d_name);
				closedir(dir);
				return absPath;
			}
		}
	}
	closedir(dir);

	std::cerr << "File not found: " << fileName << std::endl;
	return NULL;
}

std::string Gateway::extractPort(const std::string& address) {
	size_t colonPos = address.find(':');
	if (colonPos == std::string::npos) {
		return "";
	}
	return address.substr(colonPos + 1);
}

std::string Gateway::cgirun(Request& req, const string &passcgi, const string &script)
{
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		perror("pipe");
		return "";
	}

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return "";
	}

	if (pid == 0) {
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1) 
		{
			perror("dup2");
			close(pipefd[1]);
			exit(EXIT_FAILURE);
		}
		close(pipefd[1]);
		// if (strcmp(req.getMethod().c_str(), "GET") == 0)
		// {
		// 	if (dup2(pipefd[0], STDIN_FILENO) == -1) 
		// 	{
		// 		perror("dup2");
		// 		close(pipefd[0]);
		// 		exit(EXIT_FAILURE);
		// 	}
		// 	std::cout << "dwadawo" << std::endl;
		// 	std::cerr << "dwdwadwa" << std::endl;
		// 	// std::cout << req.getPacket() << std::endl;
		// }

		addenv("QUERY_STRING", req.getParams());
		addenv("REQUEST_METHOD", req.getMethod());
		addenv("SERVER_PROTOCOL", "HTTP/1.1");
		addenv("CONTENT_TYPE", req.getHeader("content-type", ""));
		addenv("SERVER_PORT", extractPort(req.getHeader("host", "")));
		addenv("HTTP_ACCEPT", req.getHeader("accept", ""));
		addenv("SERVER_NAME", req.getHeader("host", "localhost"));
		addenv("HTTP_USER_AGENT", req.getHeader("user-agent", ""));
		addenv("HTTP_ACCEPT_ENCODING", req.getHeader("accept-encoding", ""));
		addenv("HTTP_ACCEPT_LANGUAGE", req.getHeader("accept-language", "en-us"));

		// REMARQUE:
		addenv("HTTP_COOKIE", req.getHeader("cookie", "")); // AJOUT DES COOKIES
		addenv("SERVER_SOFTWARE", "Webserv/1.0.0");
		addenv("HTTP_CONNECTION", req.getHeader("connection", "keep-alive"));
		addenv("HTTP_USER_AGENT", req.getHeader("user-agent", ""));
		addenv("DOCUMENT_ROOT", "/home/michael/Documents/cursus/webserv/www");
		addenv("REQUEST_URI", req.getTarget() + (req.getParams().size() ? "?" + req.getParams() : ""));

		// REMARQUE: 
		addenv("REDIRECT_STATUS", "1"); // NECESSAIRE POUR INDIQUER AU CGI QUE C'EST BIEN UN SERVEUR QUI l'EXECUTE
		addenv("SCRIPT_FILENAME", script); // CHEMIN ABSOLU DU SCRIPT CIBLE
		addenv("PATH_INFO", req.getTarget()); // TARGET > localhost:3000/sub/test.php -> sub/test.php
		
		addenv("CONTENT_LENGTH", req.getHeader("content-length", ""));
		char** envp = put_to_env();
			char* const argv[] = { const_cast<char*>(passcgi.c_str()), NULL };
			if (execve(passcgi.c_str(), argv, envp) == -1) {
				perror("execve");
				std::cerr << "execve failed for: " << argv[0] << std::endl;
				std::cerr << "Path might be incorrect or file might not have execute permissions." << std::endl;
				std::cerr.flush();
			}
			exit(EXIT_FAILURE);
	}
	else {
		close(pipefd[1]);

		char buffer[1024];
		buffer[1023] = '\0';
		std::string response = "HTTP/1.1 200 OK\r\nContent-Length: %1\r\n";
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			response.append(buffer, bytesRead);
		}
		close(pipefd[0]);

		// REMARQUE: SET-UP LE CONTENT-LENGTH
		if (response.find("\r\n\r\n") == string::npos)
			response += "\r\n";
		response = String::replace(response, "%1", String::tostr(response.substr(response.find("\r\n\r\n") + 4).size()));
		kill(pid, SIGKILL);
		return response;
	}
}
