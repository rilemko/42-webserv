/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:25 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/17 15:42:04 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Configuration/Configuration.hpp"
#include "Foundation/Manager.hpp"
#include "Logger/Logger.hpp"
#include "Support/Extends/Abortable.hpp"
#include <signal.h>
#include <string>
#include <vector>

using namespace std;

class Application : public Abortable
{
	private  :
		string						_basedir;
		string						_cfgpath;
		int							_epollfd;
		vector<Server*>				_servers;

	public   :
		static bool					running;

	public   :
									Application(const string &cfgpath, const string &basedir = "");
									Application(const Application &src);
									~Application();

		bool						run();
		bool						exe();

		static void					end(int i);

		Application&				operator=(const Application &rhs);
};
