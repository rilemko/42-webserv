/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:25 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/30 14:23:03 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Configuration/Configuration.hpp"
#include "Gateway/Gateway.hpp"
#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Logger/Logger.hpp"
#include "Server/Server.hpp"
#include "Support/Extends/Abortable.hpp"
#include "Support/Helpers/Map.hpp"
#include "Support/Helpers/Vector.hpp"
#include "Template/Template.hpp"
#include <string>
#include <sys/epoll.h>
#include <vector>

#include <signal.h>

using namespace std;

class Application : public Abortable
{
	private  :
		string						_basedir;
		map<const int,Request*>		_chunked;
		map<const int,time_t>		_clients;
		int							_epollfd;
		vector<Server*>				_servers;

	public   :
		static bool					_running;

	public   :
									Application(const string &config, const string &basedir = "");
									Application(const Application &src);
									~Application();

		bool						run();

		static void					stop(int i);
	
	private  :
		void						add(const int &fd);
		void						end(const int &fd);
		void						handle(const int &fd);
		void						out();

	public   :
		Application&				operator=(const Application &rhs);
};
