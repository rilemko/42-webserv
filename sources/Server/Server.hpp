/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/12 16:22:16 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Logger/Logger.hpp"
#include "Network/Socket.hpp"
#include "Server/Route.hpp"
#include <map>
#include <string>

using namespace std;

class Server
{
	public   :
		int							bdsize;
		map<int,string>				errors;
		vector<string>				indexs;
		vector<Route>				routes;
		vector<string>				snames;
		int							socket;
		pair<string,string>			target;

	public   :
									Server();
									Server(const Server &src);
									~Server();

		void						run();
		bool						match() const;

		Server&						operator=(const Server &rhs);
};
