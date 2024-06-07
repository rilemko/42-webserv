/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 21:47:17 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/09 14:25:57 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

using namespace std;

class Route
{
	public   :
									Route();
									Route(const Route &src);
									~Route();

		bool						match() const;

		Route&						operator=(const Route &rhs);
};
