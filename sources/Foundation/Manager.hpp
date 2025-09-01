/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:25 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/21 13:49:23 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Gateway/Gateway.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Logger/Logger.hpp"
#include "Server/Route.hpp"
#include "Server/Server.hpp"
#include "Support/Helpers/Map.hpp"
#include "Template/Template.hpp"
#include <cstddef>
#include <map>
#include <sys/epoll.h>

using namespace std;

class Manager
{
	typedef	map<const int,pair<time_t,Request*> >	pool;

	private  :
		string						_basedir;
		map<const int,time_t>		_clients;
		int							_epollfd;
		map<const int,Gateway*>		_gateway;
		map<const int,Request*>		_pending;
		vector<Server*>				_servers;
		
	public   :
									Manager(const int &epollfd, const string &basedir, const vector<Server*> &server);
									Manager(const Manager &src);
									~Manager();
	
		void						run(const int &fd);

		void						recv(const int &fd);
		void						send(const int &fd);

		void						add(const int &fd);
		void						end(const int &fd);
		void						out();

	private  :
		void						error(Response &response, Server &server, const size_t &status);
		void						gateway();
		void						rewrite(const int &fd);
		void						upload();

	public   :
		Manager&					operator=(const Manager &rhs);
};
