/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:13 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/26 19:49:42 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Gateway/Gateway.hpp"

Gateway::Gateway()
{
}

Gateway::Gateway(const Gateway &src)
{
	*this = src;
}

Gateway::~Gateway()
{
}

size_t
Gateway::run(Request& req, const string &passcgi, const string &target)
{
	int		pipe_p2c[2], pipe_c2p[2];
	pid_t	pid;

	if (::pipe(pipe_p2c) != 0)
		return (Logger::fail("Gateway: Failed to create pipe"), 500);

	if (::pipe(pipe_c2p) == -1)
		return (::close(pipe_p2c[0]), ::close(pipe_p2c[1]), Logger::fail("Gateway: Failed to create pipe"), 500);
	
	if ((pid = ::fork()) == -1)
		 return (::close(pipe_p2c[0]), ::close(pipe_p2c[1]), ::close(pipe_c2p[0]), ::close(pipe_c2p[1]), Logger::fail("Gateway: Failed to fork CGI"), 500);
	else if (pid == 0)
    {
		::dup2(pipe_p2c[0], 0);
		::dup2(pipe_c2p[1], 1);
		::close(pipe_p2c[0]);
		::close(pipe_p2c[1]);
		::close(pipe_c2p[1]);
		::close(pipe_c2p[0]);

		Environment	env;

		env.add("REDIRECT_STATUS", "1");
		env.add("SERVER_PROTOCOL", "HTTP/1.1");
		env.add("SERVER_NAME", req.getHeader("host", "localhost"));
		env.add("SERVER_SOFTWARE", "Webserv/1.0.0");
		env.add("REQUEST_METHOD", req.getMethod());
		env.add("QUERY_STRING", req.getParams());
		env.add("CONTENT_TYPE", req.getHeader("content-type", ""));
		env.add("HTTP_ACCEPT", req.getHeader("accept", ""));
		env.add("HTTP_USER_AGENT", req.getHeader("user-agent", ""));
		env.add("HTTP_ACCEPT_ENCODING", req.getHeader("accept-encoding", ""));
		env.add("HTTP_ACCEPT_LANGUAGE", req.getHeader("accept-language", "en-us"));
		env.add("HTTP_CONNECTION", req.getHeader("connection", "keep-alive"));
		env.add("HTTP_COOKIE", req.getHeader("cookie", ""));
		env.add("REQUEST_URI", req.getTarget() + (req.getParams().size() ? "?" + req.getParams() : ""));
		env.add("SCRIPT_FILENAME", target);
		env.add("PATH_INFO", req.getTarget());
		env.add("CONTENT_LENGTH", String::tostr(req.getLength()));

		char* const agv[] = {(char*) passcgi.c_str(), (char*) target.c_str(), NULL};
        ::execve(passcgi.c_str(), agv, env);
		cerr << "error execve" << endl;
        ::exit(EXIT_FAILURE);
    }
    else
	{
		::close(pipe_p2c[0]);
        ::close(pipe_c2p[1]);

		size_t	status;

		if ((status = this->send(pipe_p2c[1], req.getPacket())) != 200)
			return (::close(pipe_p2c[1]), ::close(pipe_c2p[0]), ::kill(pid, SIGTERM), status);
		
        ::close(pipe_p2c[1]);

		if ((status = this->recv(pipe_c2p[0])) != 200)
			return (::close(pipe_c2p[0]), ::kill(pid, SIGTERM), status);

		::close(pipe_p2c[1]);
		::close(pipe_c2p[0]);
		::wait(NULL);
	}
	return (200);
}

size_t
Gateway::recv(const int &fd)
{
	timeval	timeout = {3, 0};
	fd_set	s;
	int		r;

	FD_ZERO(&s);
	FD_SET(fd, &s);

	if ((r = ::select(fd + 1, &s, NULL, NULL, &timeout)) == -1)
		return (Logger::fail("Gateway: Fail to read data from CGI"), 500);
	if (r == 0)
		return (Logger::fail("Gateway: Timeout while reading"), 504);
	if (FD_ISSET(fd, &s))
		return (this->_packet = Filesystem::read(fd), 200);
	return (500);
}

size_t
Gateway::send(const int &fd, const string &data)
{
	timeval	timeout = {3, 0};
	fd_set	s;
	int		r;

	FD_ZERO(&s);
	FD_SET(fd, &s);

	if ((r = ::select(fd + 1, NULL, &s, NULL, &timeout)) == -1)
		return (Logger::fail("Gateway: Fail to write data to CGI"), 500);
	if (r == 0)
		return (Logger::fail("Gateway: Timeout while writing"), 504);
	if (FD_ISSET(fd, &s))
		return (Filesystem::write(fd, data), 200);
	return (500);
}

string
Gateway::getPacket() const
{
	return (this->_packet);
}

Gateway&
Gateway::operator=(const Gateway &rhs)
{
	(void) rhs;
	return *this;
}
