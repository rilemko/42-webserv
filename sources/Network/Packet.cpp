/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Packet.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/12 16:26:18 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Network/Packet.hpp"

Packet::Packet(const string &packet) :
	_packet(packet)
{
}

Packet::Packet(const Packet &src)
{
	*this = src;
}

Packet::~Packet()
{
}

Packet&
Packet::operator=(const Packet &rhs)
{
	this->_packet = rhs._packet;
	return (*this);
}
