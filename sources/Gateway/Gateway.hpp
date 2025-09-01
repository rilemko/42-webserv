/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:09 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/21 21:48:11 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http/Request.hpp"
#include "Logger/Logger.hpp"
#include "Environment/Environment.hpp"
#include <cstdlib>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

class Gateway
{
	private  :
		string						_packet;

	public   :
									Gateway();
									Gateway(const Gateway &src);
									~Gateway();

		size_t						run(Request& req, const string &passcgi, const string &target);

		size_t						recv(const int &fd);
		size_t						send(const int &fd, const string &data);

		string						getPacket() const;

		Gateway&					operator=(const Gateway &rhs);


		/*std::string extractPort(const std::string& address);		
		const char *getAbsolutePathOfFile(const char *fileName);
		void	 addenv(const std::string& key, const std::string& value);
		char	**put_to_env();*/
	/*public   :
		std::vector<std::string> v;*/
};

