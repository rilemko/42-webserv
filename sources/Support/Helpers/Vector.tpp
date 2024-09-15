/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vector.tpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 22:32:44 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/17 15:14:11 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

template<typename TYPE>
bool
Vector::find(const vector<TYPE> &v, const TYPE &find)
{
	for (typename vector<TYPE>::const_iterator it = v.begin(); it != v.end(); it++)
		if (*it == find)
			return (true);
	return (false);
}

template<typename TYPE>
void
Vector::free(const vector<TYPE> &v)
{
	for (typename vector<TYPE>::const_iterator it = v.begin(); it != v.end(); it++)
		delete *it;
}
