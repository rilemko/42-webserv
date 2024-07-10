/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfiguration.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/10 17:22:25 by rdi-marz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http/Request.hpp"
#include <string>
#include <vector>

using namespace std;

class Route;

class RouteConfiguration
{
	private :
		map<string, bool>			_isDuplicate;

	public   :
									RouteConfiguration();
									RouteConfiguration(const RouteConfiguration &src);
									~RouteConfiguration();

	public   :
		void						addDirective(Route &route, const int lineNumber, const string &directive);

		RouteConfiguration&		operator=(const RouteConfiguration &rhs);

	private   :
		void _handleCgiPass(Route &route, const int lineNumber, const string &value);
		void _handleListing(Route &route, const int lineNumber, const string &value);
		void _handleIndex(Route &route, const int lineNumber, const string &value);
		void _handleMethods(Route &route, const int lineNumber, const string &value);
		void _handleRewrite(Route &route, const int lineNumber, const string &value);
		void _handleAlias(Route &route, const int lineNumber, const string &value);
		void _handleRoot(Route &route, const int lineNumber, const string &value);
		void _handleUploadTo(Route &route, const int lineNumber, const string &value);
};
