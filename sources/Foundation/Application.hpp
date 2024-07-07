/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:25 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/07 21:54:29 by mconreau         ###   ########.fr       */
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

	public   :
		static bool					_running;

	public   :
									Application(const string &cfgpath, const string &basedir = "");
									Application(const Application &src);
									~Application();

		bool						run();

		static void					end(int i);

		Application&				operator=(const Application &rhs);
};
