/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/09 18:11:12 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Datetime/Datetime.hpp"
#include "Http/Request.hpp"

using namespace std;

class Response
{
	private  :
		string						_header;
		string						_packet;
		int							_socket;

	public   :
									Response(const int &socket);
									Response(const Response &src);
									~Response();

		void						send(const string &packet);

		void						addCookie(const string &key, const string &value, const time_t &age = 0);
		void						addHeader(const string &key, const string &value);

		Response&					operator=(const Response &rhs);
};
