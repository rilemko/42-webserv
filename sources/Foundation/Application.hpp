/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:25 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/22 22:59:31 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Configuration/Configuration.hpp"
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

using namespace std;

class Application : public Abortable
{
	private  :
		map<const int,string>		_chunked;
		map<const int,time_t>		_clients;
		int							_epollfd;
		vector<Server*>				_servers;

	public   :
									Application(const string &config);
									Application(const Application &src);
									~Application();

		bool						run();
	
	private  :
		void						add(const int &fd);
		void						end(const int &fd);
		void						handle(const int &fd);
		void						out();

	public   :
		Application&				operator=(const Application &rhs);
};
