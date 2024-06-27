/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Chunk.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/27 16:28:58 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Support/Helpers/String.hpp"
#include <cstddef>

using namespace std;

class Chunk
{
	private  :
		string						_packet;
		size_t						_length;
		size_t						_status;

	public   :
									Chunk(const string &packet);
									Chunk(const Chunk &src);
									~Chunk();

		size_t						getLength() const;
		size_t						getStatus() const;

		Chunk&						operator=(const Chunk &rhs);
};
