/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:05 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/12 15:55:35 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Support/Helpers/String.hpp"
#include <netdb.h>
#include <netinet/ip.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

using namespace std;

class Socket
{
	protected:
		int							_afinet;
		int							_socket;
		pair<string,string>			_target;

	public   :
									Socket(const pair<string,string> &target, const int &type = SOCK_STREAM | SOCK_NONBLOCK);
									Socket(const Socket &src);
									~Socket();

		void						close();

		Socket&						operator=(const Socket &rhs);
									operator int() const;

		class Exception : public runtime_error
		{
			public   :
									Exception(Socket &socket, const string &message);
		};
};
