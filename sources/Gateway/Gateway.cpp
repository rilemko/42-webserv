/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:13 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/07 18:29:22 by mconreau         ###   ########.fr       */
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

Gateway&
Gateway::operator=(const Gateway &rhs)
{
	(void) rhs;
	return (*this);
}
