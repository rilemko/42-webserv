/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muteza <muteza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:13 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/14 15:28:14 by muteza           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

char **Gateway::addenv()
{
		char **envp = new char*[v.size() + 1];
		size_t i;

		for (i = 0; i < v.size(); ++i)
			envp[i] = v[i];
		envp[i] = 0;
		return (envp);
}

void	Gateway::cgirun()
{
	
}

