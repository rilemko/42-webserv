/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/14 19:55:20 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Datetime/Datetime.hpp"
#include "Http/HttpStatus.hpp"
#include "Http/Request.hpp"

using namespace std;

class Response
{
	private  :
		string						_header;
		string						_packet;
		int							_socket;
		size_t						_status;

	public   :
									Response(const int &socket);
									Response(const Response &src);
									~Response();

		size_t						send();

		void						addCookie(const string &key, const string &value, const time_t &age = 0);
		void						addHeader(const string &key, const string &value);
		void						addPacket(const string &packet);
		void						setStatus(const size_t &status);

		Response&					operator=(const Response &rhs);
};
