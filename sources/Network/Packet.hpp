/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Packet.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/12 16:26:10 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

using namespace std;

class Packet
{
	protected:
		string						_packet;

	public   :
									Packet(const string &packet);
									Packet(const Packet &src);
									~Packet();

		Packet&						operator=(const Packet &rhs);
};
