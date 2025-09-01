/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Template.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 19:48:59 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/18 22:58:10 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Template/Template.hpp"

Template::~Template()
{
}

string
Template::error(const int &status)
{
	static HttpStatus	HttpStatus;
	const string		c = (status < 300 ? "27ae60" : "e74c3c");
	const string		packet = "\
<!DOCTYPE html>\
<html lang=\"fr\">\
	<head>\
		<meta charset=\"utf-8\">\
		<title>%1 %2</title>\
	</head>\
	<body style=\"display: flex; flex-direction: column; align-items: center; margin: 0; padding: 64px; min-height: 500px; background-color: #%3; font-family: sans-serif; box-sizing: border-box;\">\
		<div style=\"top: 128px; display: flex; align-items: center; flex-direction: column; gap: 16px;\">\
			<div style=\"color: #e3e3e3; font-size: 64px; font-weight: bold;\">%1</div>\
			<div style=\"color: #e3e3e3; font-size: 32px;\"><center>%2</center></div>\
		</div>\
		<div style=\"bottom: 32px; left: 50%; transform: translateX(-50%); font-size: 18px; font-weight: bold; color: #e3e3e3; position: absolute;\">WEBSERV 1.0.0</div>\
	</body>\
</html>";

	return (String::replace(String::replace(String::replace(packet, "%1", String::tostr(status)), "%2", HttpStatus.get(status)), "%3", c));
}

#include <iostream>

string
Template::index(const string &target, const string &root)
{
	vector<string>	files = Filesystem::scandir(target);
	string			list, packet("\
<!DOCTYPE html>\
<html lang=\"fr\">\
	<head>\
		<meta charset=\"utf-8\">\
		<title>Webserv</title>\
	</head>\
	<body style=\"display: flex; flex-direction: column; margin: 0; padding: 64px; min-height: 500px; background-color: #2980b9; font-family: sans-serif; box-sizing: border-box;\">\
		<div style=\"padding: 16px; margin: 16px auto; width: 75%; background-color: #e3e3e3; display: flex; flex-direction: column; gap: 6px; border-radius: 4px;\">\
			%2\
		</div>\
		<div style=\"bottom: 32px; left: 50%; transform: translateX(-50%); font-size: 18px; font-weight: bold; color: #e3e3e3; position: absolute;\">WEBSERV 1.0.0</div>\
	</body>\
</html>");

	for (size_t i = 0; i < files.size(); i++)
		list += "<a href=\"" + root + "/" + files[i] + "\">" + files[i] + (Filesystem::isDir(files[i]) ? "/" : "") + "</a>";
	return (String::replace(packet, "%2", list));
}
