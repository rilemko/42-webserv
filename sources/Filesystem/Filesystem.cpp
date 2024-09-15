/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Filesystem.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 13:02:11 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/18 11:41:19 by mconreau         ###   ########.fr       */
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
Filesystem::extension(const string &path)
{
	size_t	pos;
	return ((pos = path.rfind('.')) != string::npos ? path.substr(pos) : "");
}

string
Filesystem::mimetype(const string &path)
{
	static MimeType	Mime;
	return (Mime.get(Filesystem::extension(path)));
}

string
Filesystem::get(const string &path)
{
	ostringstream 	oss;

	oss << ifstream(path.c_str(), ios::binary | ios::in).rdbuf();
	return (oss.str());
}

void
Filesystem::set(const string &path, const string &data)
{
	ofstream(path.c_str(), ios::binary | ios::out).write(data.c_str(), data.size());
}

string
Filesystem::normalize(const string &path)
{
	const size_t	len = path.size();
	string			result;

	for (size_t i = 0; i < len; i++)
	{
		if (i < len - 1 && path[i] == '/' && path[i + 1] == '/')
			continue ;
		result += path[i];
	}
	return (result);
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

bool
Filesystem::remove(const string &path)
{
	return (Filesystem::isReg(path) && ::remove(path.c_str()) == 0);
}

string
Filesystem::recv(const int &fd)
{
	char	buffer[1024];
	string	data;

	for (int b = 0; (b = ::recv(fd, buffer, 1024, MSG_DONTWAIT)) > 0;)
		data.append(buffer, b);
	return (data);
}

string
Filesystem::read(const int &fd)
{
	char	buffer[1024];
	string	data;

	for (int b = 0; (b = ::read(fd, buffer, 1024)) > 0;)
		data.append(buffer, b);
	return (data);
}

void
Filesystem::send(const int &fd, const string &data)
{
	const int	l = data.size();

	for (int b = 0, r = l, t = 0; t < l;)
	{
		r -= (b = ::send(fd, &data[t], r, MSG_DONTWAIT));
		if (b < 1) return ;
		t += b;
	}
}

void
Filesystem::write(const int &fd, const string &data)
{
	const int	l = data.size();

	for (int b = 0, r = l, t = 0; t < l;)
	{
		r -= (b = ::write(fd, &data[t], r));
		if (b < 1) return ;
		t += b;
	}
}

vector<string>
Filesystem::scandir(const string &path, const bool &hidden)
{
	vector<string>	scan;
	DIR				*dir;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		for (struct dirent *dp; (dp = readdir(dir));)
		{
			string	target(dp->d_name);

			if (target != "." && target != ".." && (hidden || target[0] != '.'))
				scan.push_back(target);
		}
	}
	closedir(dir);
	return (scan);
}
