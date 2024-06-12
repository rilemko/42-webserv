/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vector.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 22:30:54 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/12 21:42:39 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

using namespace std;

class Vector
{
	private  :
		virtual						~Vector() = 0;

	public   :

		template<typename TYPE>
		static void					free(vector<TYPE> &vector);
};

#include "Support/Helpers/Vector.tpp"
