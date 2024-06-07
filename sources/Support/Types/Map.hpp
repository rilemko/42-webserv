/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 20:29:37 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/08 16:23:45 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Support/Extends/Mappable.hpp"

template<typename KEY, typename VAL>
class Map : public Mappable<KEY,VAL>
{
};
