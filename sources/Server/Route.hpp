/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/02 14:51:40 by rdi-marz         ###   ########.fr       */
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

	private :
		map<string, bool>			isDuplicate;

	public   :
									Route();
									Route(const Route &src);
									~Route();

		void						addDirective(const int lineNumber, const string &directive);
		void						printRoute(void) const;

		size_t						check(Request &request) const;
		bool						match(Request &request) const;

		Route&						operator=(const Route &rhs);

	private   :
		void handleCgiPass(const int lineNumber, const string &value);
		void handleListing(const int lineNumber, const string &value);
		void handleIndex(const int lineNumber, const string &value);
		void handleMethods(const int lineNumber, const string &value);
		void handleRewrite(const int lineNumber, const string &value);
		void handleRoot(const int lineNumber, const string &value);
		void handleUploadTo(const int lineNumber, const string &value);
};
