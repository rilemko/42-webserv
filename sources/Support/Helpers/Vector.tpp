/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vector.tpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/26 22:32:44 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/12 21:42:33 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

template<typename TYPE>
void
Vector::free(vector<TYPE> &vector)
{
	for (size_t i = 0; i < vector.size(); i++)
		delete vector[i];
}
