/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muteza <muteza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/14 17:41:36 by muteza           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Filesystem/Filesystem.hpp"
#include "Support/Extends/Abortable.hpp"
#include "Support/Helpers/String.hpp"
#include <map>
#include <string>

using namespace std;

class Request : public Abortable
{
	private  :
		map<string,string>			_header;
		size_t						_length;
		string						_method;
		string						_packet;
		string						_params;
		int							_socket;
		string						_target;


	public   :
									Request(const int &socket);
									Request(const Request &src);
									~Request();

		size_t						recv();

		string						_version;
		string						getHeader(const string &key, const string &other);
		size_t						getLength() const;
		string						getMethod() const;
		string						getPacket() const;
		string						getParams() const;
		string						getTarget() const;

		Request&					operator=(const Request &rhs);
};
