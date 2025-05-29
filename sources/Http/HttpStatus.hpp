/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 18:05:30 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/18 19:10:21 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

using namespace std;

class HttpStatus
{
	private  :
		map<size_t,string>			_status;

	public   :
									HttpStatus();
									HttpStatus(const HttpStatus &src);
									~HttpStatus();

		string						get(const size_t &status);

		HttpStatus&					operator=(const HttpStatus &rhs);
};
