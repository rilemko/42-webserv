/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/17 14:53:09 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server/Route.hpp"

Route::Route() 
{
}

Route::Route(const Route &src)
{
	*this = src;
}

Route::~Route()
{
}

size_t
Route::check(Request &request) const
{
	if (!Vector::find<string>(this->_method, request.getMethod()))
		return (405);
	return (200);
}

bool
Route::match(Request &request) const
{
	return (this->_target == request.getTarget());
}

Route&
Route::operator=(const Route &rhs)
{
	(void) rhs;
	return (*this);
}
