/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/20 17:43:26 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http/Request.hpp"
#include "Support/Helpers/Vector.hpp"
#include <string>
#include <vector>

using namespace std;

class Route
{
	public   :
		string						passcgi;
		bool						dirlst;
		string						dindex;
		vector<string>				method;
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
