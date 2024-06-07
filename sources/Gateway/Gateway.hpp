/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Gateway.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 18:27:09 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/07 18:29:50 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

using namespace std;

class Gateway
{
	public   :
									Gateway();
									Gateway(const Gateway &src);
									~Gateway();

		Gateway&					operator=(const Gateway &rhs);
};
