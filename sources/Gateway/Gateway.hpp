/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muteza <muteza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:09 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/14 15:28:29 by muteza           ###   ########.fr       */
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
		char	**addenv();
		void	cgirun();
	public   :
		std::vector<char *> v;
		std::string	a;
};

