/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muteza <muteza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:13 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/29 17:10:52 by muteza           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Gateway/Gateway.hpp"
#include "Http/Request.hpp"

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

std::string Gateway::cgirun(Request& req, std::string passcgi) {
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
		if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
			perror("dup2");
			close(pipefd[1]);
			exit(EXIT_FAILURE);
		}
		close(pipefd[1]);

		addenv("QUERY_STRING", req.getTarget());
		addenv("REQUEST_METHOD", req.getMethod());
		addenv("SERVER_PROTOCOL", "HTTP/1.1");
		addenv("CONTENT_TYPE", req.getHeader("content-type",""));
		addenv("CONTENT_LENGTH", req.getHeader("content-length",""));
		addenv("HTTP_ACCEPT", req.getHeader("accept",""));
		addenv("SERVER_NAME", req.getHeader("host",""));
		addenv("HTTP_USER_AGENT", req.getHeader("user-agent",""));
		addenv("HTTP_ACCEPT_ENCODING", req.getHeader("accept-encoding",""));
		addenv("HTTP_ACCEPT_LANGUAGE", req.getHeader("accept-language",""));
		char** envp = put_to_env();

		std::vector<std::string> paths;
		const char* absPath = getAbsolutePathOfFile(passcgi.c_str());
   		if (absPath != NULL) 
	 	{
			char* const argv[] = { const_cast<char*>(absPath), NULL };
			if (execve(absPath, argv, envp) == -1) {
				perror("execve");
				std::cerr << "execve failed for: " << argv[0] << std::endl;
				std::cerr << "Path might be incorrect or file might not have execute permissions." << std::endl;
				std::cerr.flush();
			}
				exit(EXIT_FAILURE);
		}
		return (NULL);
	}
	else {
		close(pipefd[1]);

		char buffer[1024];
		buffer[1023] = '\0';
		std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
			response.append(buffer, bytesRead);
		}
		close(pipefd[0]);

		int status;
		waitpid(pid, &status, 0); // Wait for child process to finish
		if (WIFEXITED(status)) {
			return response;
		} else if (WIFSIGNALED(status)) {
			std::cerr << "Child killed by signal: " << WTERMSIG(status) << std::endl;
		}
		return response;
	}
}
