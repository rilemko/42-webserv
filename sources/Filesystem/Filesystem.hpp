/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Filesystem.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 16:42:59 by mconreau          #+#    #+#             */
/*   Updated: 2024/04/13 16:55:49 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

class Filesystem
{
	private  :
		virtual						~Filesystem() = 0;

	public   :
		static bool					exist(const string &path);
		static bool					isDir(const string &path);
		static bool					isLnk(const string &path);
		static bool					isReg(const string &path);
		static bool					isSck(const string &path);
		static bool					isReadable(const string &path);
		static bool					isWritable(const string &path);
		static bool					isXcutable(const string &path);
};
