/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 20:16:33 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/04 22:49:36 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Logger/Logger.hpp"
#include "Server/Server.hpp"
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
		string						reformatLine(const std::string &line, int sourceLineNumber);

	public   :
		void						printConfig(void) const;

		Configuration&				operator=(const Configuration& rhs);
									operator vector<Server*>() const;
};
