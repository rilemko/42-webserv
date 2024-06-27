/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chunk.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/27 16:14:04 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Chunk.hpp"

Chunk::Chunk(const string &packet) :
	_status(200)
{
	if (!String::match("?*\r\n*\r\n", packet))
		this->_status = 1;
	else
	{
		
	}
	
}

Chunk::Chunk(const Chunk &src)
{
	*this = src;
}

Chunk::~Chunk()
{
}

size_t
Chunk::getLength() const
{
	return (this->_status);
}

size_t
Chunk::getStatus() const
{
	return (this->_status);
}

Chunk&
Chunk::operator=(const Chunk &rhs)
{
	(void) rhs;
	return (*this);
}
