/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 17:56:03 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/07 15:44:44 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Foundation/Application.hpp"
#include "Logger/Logger.hpp"

int
main(const int agc, const char **agv)
{
	Logger::logs("Launching Webserv...");
	return(!Application(agc > 1 ? agv[1] : "").run());
}
