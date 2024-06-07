/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:19 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/09 19:54:21 by mconreau         ###   ########.fr       */
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

bool
Route::match() const
{
	return (true);
}

Route&
Route::operator=(const Route &rhs)
{
	(void) rhs;
	return (*this);
}
