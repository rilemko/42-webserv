/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:25 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/20 18:01:04 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http/Request.hpp"
#include "Http/Response.hpp"
#include "Server/Route.hpp"
#include "Server/Server.hpp"
#include "Template/Template.hpp"
#include <string>

using namespace std;

class Handler
{
	private  :


	public   :
									Handler(const string &config);
									Handler(const Handler &src);
									~Handler();

		bool						run(const int &fd);
	

	public   :
		Handler&					operator=(const Handler &rhs);
};
