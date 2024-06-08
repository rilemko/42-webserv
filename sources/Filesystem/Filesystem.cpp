/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Filesystem.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 13:02:11 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/08 20:44:25 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Filesystem/Filesystem.hpp"

Filesystem::~Filesystem()
{
}

bool
Filesystem::exist(const std::string &path)
{
	struct stat	st;
	return (!stat(path.c_str(), &st));
}

string
Filesystem::get(const string &path)
{
	ostringstream 	oss;

	oss << ifstream(path.c_str(), ios::in | ios::binary).rdbuf();
	return (oss.str());
}

void
Filesystem::set(const string &path, const string &data)
{
	ofstream(path.c_str(), ios::out | ios::binary) << data.c_str();
}

bool
Filesystem::isDir(const std::string &path)
{
	struct stat	st;
	return (!stat(path.c_str(), &st) && S_ISDIR(st.st_mode));
}

bool
Filesystem::isLnk(const std::string &path)
{
	struct stat	st;
	return (!stat(path.c_str(), &st) && S_ISLNK(st.st_mode));
}

bool
Filesystem::isReg(const std::string &path)
{
	struct stat	st;
	return (!stat(path.c_str(), &st) && S_ISREG(st.st_mode));
}

bool
Filesystem::isSck(const std::string &path)
{
	struct stat	st;
	return (!stat(path.c_str(), &st) && S_ISSOCK(st.st_mode));
}

bool
Filesystem::isReadable(const std::string &path)
{
	return (!access(path.c_str(), R_OK));
}

bool
Filesystem::isWritable(const std::string &path)
{
	return (!access(path.c_str(), W_OK));
}

bool
Filesystem::isXcutable(const std::string &path)
{
	return (!access(path.c_str(), X_OK));
}
