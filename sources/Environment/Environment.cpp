/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Environment.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 18:00:14 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/30 14:33:01 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Environment/Environment.hpp"

Environment::Environment()
{
}

Environment::Environment(const char **env)
{
	if (env)
	{
		for (;*env; env++)
		{
			const string	s(*env);
			const int		f = s.find('=');
			
			this->add(s.substr(0, f), s.substr(f + 1));
		}
	}
}

Environment::Environment(const Environment &src) :
	Mappable(src)
{
	*this = src;
}

Environment::~Environment()
{
}

Environment&
Environment::operator=(const Environment &rhs)
{
	Mappable::operator=(rhs);
	return (*this);
}

Environment::operator char**() const
{
	char	**env = new char*[this->size() + 1];
	int		i = 0;

	for (map<string,string>::const_iterator it = this->_map.begin(); it != this->_map.end(); it++, i++)
	{
		const string	s = (*it).first + "=" + (*it).second;
		const int		l = s.size();

		env[i] = new char[l + 1];
		for (int j = 0, k = s.size(); j < k; j++)
			env[i][j] = s[j];
		env[i][l] = '\0';
	}
	env[i] = NULL;
	return (env);
}
