/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/18 16:14:44 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Filesystem/Filesystem.hpp"
#include "Support/Extends/Abortable.hpp"
#include "Support/Helpers/String.hpp"
#include <map>
#include <string>

using namespace std;

class Request
{
	private  :
		string						_chunkr;
		map<string,string>			_cookie;
		map<string,string>			_header;
		size_t						_length;
		string						_method;
		string						_packet;
		string						_params;
		size_t						_remain;
		string						_sessid;
		int							_socket;
		size_t						_status;
		string						_target;
		map<string,string>			_upload;

	public   :
									Request(const int &socket);
									Request(const Request &src);
									~Request();

	public   :
		void						recv();
		void						unbound(const string &packet);
		void						unchunk(const string &packet);

		string						getCookie(const string &key, const string &other);
		string						getHeader(const string &key, const string &other);
		size_t						getLength() const;
		string						getMethod() const;
		string						getPacket() const;
		string						getParams() const;
		string						getTarget() const;
		int							getSocket() const;
		size_t						getStatus() const;
		map<string,string>			getUpload() const;
	
	private  :
		void						setStatus(const size_t &status);

	public   :
		Request&					operator=(const Request &rhs);
};
