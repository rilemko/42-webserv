/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.tpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 22:32:44 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/11 18:29:01 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

template<typename KEY, typename VAL>
void
Map<KEY,VAL>::freeKey(map<KEY,VAL> &m)
{
	for (typename map<KEY,VAL>::const_iterator it = m.begin(); it != m.end(); it++)
		delete (*it).first;
}

template<typename KEY, typename VAL>
void
Map<KEY,VAL>::freeVal(map<KEY,VAL> &m)
{
	for (typename map<KEY,VAL>::const_iterator it = m.begin(); it != m.end(); it++)
		delete (*it).second;
}

template<typename KEY, typename VAL>
template<typename R>
void
Map<KEY,VAL>::iterKey(map<KEY,VAL> &m, R (*callback)(KEY))
{
	for (typename map<KEY,VAL>::iterator it = m.begin(); it != m.end(); it++)
		callback((*it).first);
}

template<typename KEY, typename VAL>
template<typename R>
void
Map<KEY,VAL>::iterVal(map<KEY,VAL> &m, R (*callback)(VAL))
{
	for (typename map<KEY,VAL>::iterator it = m.begin(); it != m.end(); it++)
		callback((*it).second);
}
