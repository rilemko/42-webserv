/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 20:29:37 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/11 17:59:35 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Support/Extends/Mappable.hpp"
#include <map>

template<typename KEY, typename VAL>
class Map : public Mappable<KEY,VAL>
{
	private  :
		virtual						~Map() = 0;
	
	public   :
		static void					freeKey(map<KEY,VAL> &m);
		static void					freeVal(map<KEY,VAL> &m);

		template<typename R>
		static void					iterKey(map<KEY,VAL> &m, R (*callback)(KEY));
		template<typename R>
		static void					iterVal(map<KEY,VAL> &m, R (*callback)(VAL));
};

#include "Support/Helpers/Map.tpp"
