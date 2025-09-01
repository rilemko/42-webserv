/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 20:16:33 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/26 19:48:08 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Logger/Logger.hpp"
#include "Server/Server.hpp"
#include "Configuration/RouteConfiguration.hpp"
#include "Configuration/ServerConfiguration.hpp"
#include "Support/Extends/Abortable.hpp"
#include "Support/Helpers/String.hpp"
#include <fstream>
#include <sys/epoll.h>
#include <vector>

class Configuration : public Abortable
{
	private  :
		vector<Server*>				_servers;
		bool						_isNotRoute;

	public   :
									Configuration(const string &config, const int &epollfd);
									Configuration(const Configuration& src);
									~Configuration();
	private  :
		string						_reformatLine(const string &line, int sourceLineNumber);

	public   :
		Configuration&				operator=(const Configuration& rhs);
									operator vector<Server*>() const;
};
