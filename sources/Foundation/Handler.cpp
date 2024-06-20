/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/20 18:00:47 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Handler.hpp"

Handler::Handler(const string &config)
{
}

Handler::Handler(const Handler &src)
{
	*this = src;
}

Handler::~Handler()
{
}

bool
Handler::run(const int &fd)
{
	(void) fd;
}

Handler&
Handler::operator=(const Handler &rhs)
{
	(void) rhs;
	return (*this);
}
