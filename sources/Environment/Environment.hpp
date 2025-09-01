/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Environment.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:25 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/30 14:28:04 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Support/Extends/Mappable.hpp"
#include <cstddef>
#include <map>
#include <string>
#include <vector>

using namespace std;

class Environment : public Mappable<string,string>
{
	public   :
									Environment();
									Environment(const char **env);
									Environment(const Environment &src);
									~Environment();

	public   :
		Environment&				operator=(const Environment &rhs);
									operator char**() const;
};
