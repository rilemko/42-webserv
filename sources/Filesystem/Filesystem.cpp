/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Filesystem.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 13:02:11 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/07 18:19:21 by mconreau         ###   ########.fr       */
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

string
Filesystem::recv(const int &fd)
{
	string	data;
	char	buffer[2048];

	for (int b = 0; (b = ::recv(fd, buffer, 2048, MSG_DONTWAIT)) > 0;)
		data.append(buffer, b);
	return (data);
}

void
Filesystem::send(const int &fd, const string &data)
{
	const int	l = data.size();
	int 		b = 0, r = l, t = 0;

	while (t < l)
	{
		r -= (b = ::send(fd, &data[t], r, MSG_DONTWAIT));
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
