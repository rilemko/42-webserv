/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:09 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/30 14:41:14 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdlib> // For EXIT_FAILURE
#include <cstring> // For strrerror
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include "Http/Request.hpp"
#include <string>

class Gateway
{
	public   :
		Gateway();
		Gateway(const Gateway &src);
		~Gateway();
		Gateway&	operator=(const Gateway &rhs);

		
		const char *getAbsolutePathOfFile(const char *fileName);
		void	 addenv(const std::string& key, const std::string& value);
		char	**put_to_env();
		std::string	cgirun(Request& req, const string &passcgi, const string &script);
	public   :
		std::vector<std::string> v;
};

