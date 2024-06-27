/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/25 13:24:30 by mconreau         ###   ########.fr       */
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

		void						addDirective(const string &directive);
		void						printRoute(void) const;

		size_t						check(Request &request) const;
		bool						match(Request &request) const;

		Route&						operator=(const Route &rhs);

	private   :
		void handleCgiPass(const string &value);
		void handleListing(const string &value);
		void handleIndex(const string &value);
		void handleMethods(const string &value);
		void handleRewrite(const string &value);
		void handleRoot(const string &value);
		void handleUploadTo(const string &value);
};
