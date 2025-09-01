/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfiguration.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/16 08:31:47 by rdi-marz         ###   ########.fr       */
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
		static map<string, bool>	_isDuplicate;
		static bool					_isDuplicateInitialized;

	public   :
		static void					addDirective(Server &server, const int lineNumber, const string &directive);
		static void					resetInitialization(void);

	private :
									ServerConfiguration();
									~ServerConfiguration();
		static void 				initIsDuplicate(void);
		static bool					contains(const vector<string>& vec, const string& value);
		static void					handleErrorsPage(Server &server, const int lineNumber, const string &value);
		static void					handleListen(Server &server, const int lineNumber, const string &value);
		static void					handleMaxBodySize(Server &server, const int lineNumber, const string &value);
		static void					handleServerName(Server &server, const int lineNumber, const string &value);
};
