/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/17 22:12:45 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http/Request.hpp"
#include "Logger/Logger.hpp"
#include "Network/Socket.hpp"
#include "Support/Helpers/String.hpp"
#include "Support/Helpers/Vector.hpp"
#include "Server/Route.hpp"
#include <map>
#include <string>

using namespace std;

class Server
{
	public   :
		map<int,string>				errors;
		pair<string,string>			listen;
		size_t						maxbdy;
		vector<Route*>				routes;
		vector<string>				snames;
		int							socket;
		pair<in_addr_t,in_port_t>	target;

	public   :
									Server();
									Server(const Server &src);
									~Server();

		void						run();

		size_t						check(Request &request) const;
		bool						match(Request &request) const;

		Server&						operator=(const Server &rhs);
};
