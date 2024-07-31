/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfiguration.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdi-marz <rdi-marz@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/16 08:35:24 by rdi-marz         ###   ########.fr       */
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
	private  :
		static map<string, bool>	_isDuplicate;
		static bool					_isDuplicateInitialized;

	public   :
		static void					addDirective(Route &route, const int lineNumber, const string &directive);
		static void					resetInitialization(void);

	private  :
									RouteConfiguration();
									~RouteConfiguration();
		static void 				initIsDuplicate(void);
		static void					handleCgiPass(Route &route, const int lineNumber, const string &value);
		static void					handleListing(Route &route, const int lineNumber, const string &value);
		static void					handleIndex(Route &route, const int lineNumber, const string &value);
		static void					handleMethods(Route &route, const int lineNumber, const string &value);
		static void					handleRewrite(Route &route, const int lineNumber, const string &value);
		static void					handleAlias(Route &route, const int lineNumber, const string &value);
		static void					handleRoot(Route &route, const int lineNumber, const string &value);
		static void					handleUploadTo(Route &route, const int lineNumber, const string &value);
};
