/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muteza <muteza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:09 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/14 17:38:06 by muteza           ###   ########.fr       */
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
		void	cgirun(Request	req);
	public   :
		std::vector<char *> v;
		std::string	a;
};

