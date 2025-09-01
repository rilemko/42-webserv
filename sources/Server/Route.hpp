/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/26 19:25:43 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http/Request.hpp"
#include "Support/Helpers/Vector.hpp"
#include <string>
#include <vector>

class RouteConfiguration;

using namespace std;

class Route
{
	public   :
		string						alias;
		bool						dirlst;
		string						dindex;
		vector<string>				method;
		string						passcgi;
		pair<size_t,string>			rewrite;
		string						rooting;
		string						target;
		string						upload;

	public   :
									Route();
									Route(const Route &src);
									~Route();

		size_t						check(Request &request) const;
		bool						match(Request &request) const;

		Route&						operator=(const Route &rhs);
};
