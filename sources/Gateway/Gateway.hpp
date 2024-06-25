/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:09 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/21 20:10:47 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Http/Request.hpp"
#include <string>

class Gateway
{
	public   :
		Gateway();
		Gateway(const Gateway &src);
		~Gateway();
		Gateway&	operator=(const Gateway &rhs);

		void addenv(std::string key, std::string value);
		char	**put_to_env();
		std::string	cgirun(Request	req, std::string cgi);
	public   :
		std::vector<char *> v;
		std::string	a;
};

