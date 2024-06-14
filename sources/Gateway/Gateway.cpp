/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muteza <muteza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:13 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/14 17:42:17 by muteza           ###   ########.fr       */
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

void	Gateway::cgirun(Request	req)
{
	char **envp;
	addenv("REQUEST_METHOD=",req.getMethod());
	addenv("SERVER_PROTOCOL=",req._version);
	// addenv("REQUEST_METHOD=",req.getMethod());
	// addenv("REQUEST_METHOD=",req.getMethod());
	// addenv("CONTENT_LENGTH=",req.getLength(), req);
	envp = put_to_env();
	std::cout << envp[0] << std::endl;
}

