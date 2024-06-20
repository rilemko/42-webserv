/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:13 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/20 18:20:26 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "Gateway/Gateway.hpp"
#include "Http/Request.hpp"

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

Gateway& Gateway::operator=(const Gateway &rhs)
{
	(void) rhs;
	return (*this);
}

void	Gateway::addenv(std::string key, std::string value)
{
	//lenght

	std::string str = key + value;
	v.push_back(strdup(str.c_str()));

	// For adding char* to vector. But it's better to add const char* to a vector<const char*> anyway for performance
	// v.push_back(&str[0]);
}

char **Gateway::put_to_env()
{
		char **envp = new char*[v.size() + 1];
		size_t i;

		for (i = 0; i < v.size(); ++i)
			envp[i] = v[i];
		envp[i] = 0;
		return (envp);
}

string	Gateway::cgirun(Request	req, string cgi)
{
	char **envp;

	(void) cgi;
	(void) envp;
	addenv("REQUEST_METHOD=",req.getMethod());
	addenv("SERVER_PROTOCOL=",req.getPacket());
	addenv("Co=",req.getMethod());
	// addenv("REQUEST_METHOD=",req.getMethod());
	// addenv("CONTENT_LENGTH=",req.getLength(), req);
	envp = put_to_env();
	//std::cout << envp[0] << std::endl;
	return ("");
}

