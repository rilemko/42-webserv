/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   String.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 16:06:15 by mconreau          #+#    #+#             */
/*   Updated: 2024/05/13 21:07:31 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Support/Helpers/String.hpp"

String::~String()
{
}

string
String::replace(string subject, const string &from, const string &to)
{
	const int	len = from.size();
	size_t		pos = 0;

	while ((pos = subject.find(from, pos)) != string::npos)
	{
		subject.replace(pos, len, to);
		pos += len;
	}
	return (subject);
}

bool
String::match(const string &pattern, const string &subject)
{
	const int 	n = subject.length(), m = pattern.length();
	int			i = 0, j = 0, last = -1, match = 0;

	while (i < n)
	{
		if (j < m && (pattern[j] == '?' || pattern[j] == subject[i]))
			++i, ++j;
		else if (j < m && pattern[j] == '*')
		{
			last = j++;
			match = i;
		}
		else if (last != -1)
		{
			j = last + 1;
			i = ++match;
		}
		else
			return (false);
	}
	while (j < m && pattern[j++] == '*');
	return (j == m);
}

string
String::ltrim(string subject, const string &remove)
{
	return (subject.erase(0, subject.find_first_not_of(remove)));
}

string
String::rtrim(string subject, const string &remove)
{
	return (subject.erase(subject.find_last_not_of(remove) + 1));
}

string
String::strim(const string &subject, const string &remove)
{
	return (String::rtrim(String::ltrim(subject, remove), remove));
}

#include <iostream>

vector<string>
String::split(const string &subject, const string &delimiter)
{
	vector<string>	array;
	size_t			end = 0, start = 0, len = delimiter.length();

	while ((end = subject.find(delimiter, start)) != string::npos)
	{
		array.push_back(subject.substr(start, end - start));
		start = end + len;
	}
	return (array);
}

string
String::join(const vector<string> &array, const string &delimiter)
{
	string	content;

	for (vector<string>::const_iterator it = array.begin(), end = array.end(); it != end; it++)
	{
		content += *it;
		if (it + 1 != end)
			content += delimiter;
	}
	return (content);
}

string
String::lowercase(string subject)
{
	for (int i = 0, j = subject.size(); i < j; i++)
		subject[i] = tolower(subject[i]);
	return (subject);
}

string
String::uppercase(string subject)
{
	for (int i = 0, j = subject.size(); i < j; i++)
		subject[i] = toupper(subject[i]);
	return (subject);
}
