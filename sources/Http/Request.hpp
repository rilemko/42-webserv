/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/08 22:28:15 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Support/Extends/Abortable.hpp"
#include "Support/Extends/Mappable.hpp"
#include "Support/Helpers/String.hpp"
#include "Support/Types/Map.hpp"
#include <sys/socket.h>
#include <string>

using namespace std;

class Request : public Abortable, public Mappable<string,string>
{
	private  :
		string						_method;
		string						_packet;
		string						_params;
		int							_socket;

		string						_body;
		size_t						_size;
		string						_uri;
		string						_version;

	public   :
									Request(const int &socket);
									Request(const Request &src);
									~Request();

		void						recv();

		Request&					operator=(const Request &rhs);
};
