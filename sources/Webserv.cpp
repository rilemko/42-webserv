/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 17:56:03 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/15 13:02:45 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Environment/Environment.hpp"
#include "Foundation/Application.hpp"
#include "Logger/Logger.hpp"

#include "Filesystem/Filesystem.hpp"

int
main(const int agc, const char **agv, const char **env)
{
	Logger::logs("Launching Webserv...");
	return(!Application(agc > 1 ? agv[1] : "", Environment(env).get("PWD", "")).run());
}

/*
#include "Filesystem/Filesystem.hpp"
#include <chrono>
#include <iostream>
#include "Environment/Environment.hpp"

int
main(const int agc, const char **agv, const char **env)
{
	(void) agc;
	(void) agv;
	(void) env;

	using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	// ================================================================================

	Environment	e(env);

	e.add("test", "fijjhsifhsqdo");

	//cout << e.get("_", "X") << endl;

	char	**a = e;
	(void) a;

	// ================================================================================
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

	for (;*a; a++)
	{
		cout << *a << endl;
	}

	return (0);
}
*/
