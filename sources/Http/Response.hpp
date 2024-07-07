/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/07 22:49:02 by mconreau         ###   ########.fr       */
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
		string						_sessid;
		int							_socket;
		size_t						_status;

	public   :
									Response(const int &socket);
									Response(const Response &src);
									~Response();

		void						send();
		void						send(const string &packet);

		Response&					addCookie(const string &key, const string &value, const time_t &age = 0);
		Response&					addHeader(const string &key, const string &value);
		Response&					addPacket(const string &packet);
		Response&					setSessid(const string &sessid);
		Response&					setStatus(const size_t &status);

		Response&					operator=(const Response &rhs);
};
