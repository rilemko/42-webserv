/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 17:21:23 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Support/Helpers/Vector.hpp"
#include <map>
#include <string>

using namespace std;

class Server;

class ServerConfiguration
{

	private :
		map<string, bool>			_isDuplicate;

	public   :
									ServerConfiguration();
									ServerConfiguration(const ServerConfiguration &src);
									~ServerConfiguration();

		void						addDirective(Server &server, const int lineNumber, const string &directive);

		ServerConfiguration&		operator=(const ServerConfiguration &rhs);

	private   :
		bool						_contains(const vector<string>& vec, const string& value);
		void						_handleErrorsPage(Server &server, const int lineNumber, const string &value);
		void						_handleListen(Server &server, const int lineNumber, const string &value);
		void						_handleMaxBodySize(Server &server, const int lineNumber, const string &value);
		void						_handleServerName(Server &server, const int lineNumber, const string &value);
};
