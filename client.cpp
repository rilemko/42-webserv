/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/22 20:09:54 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/07 18:55:08 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>

using namespace std;

string	recv(const int &fd);
void	send(const int &fd, const string &data);

int
main(void)
{
	const int 	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	hostent*	server = ::gethostbyname("0.0.0.0");
	sockaddr_in	addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = ::htons(3000);
    ::bcopy(server->h_addr, &addr.sin_addr.s_addr, static_cast<size_t>(server->h_length));

	if (::connect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
		return (perror("connect"), 1);

	send(socket, "POST / HTTP/1.1\r\nHost: 0.0.0.0:3000\r\nContent-Type: multipart/form-data; boundary=test\r\nContent-Length: 12\r\n\r\nHello");
	sleep(1);
	send(socket, " world!");

	/*send(socket, "POST / HTTP/1.1\r\nHost: 0.0.0.0:3000\r\nTransfer-Encoding: chunked\r\nContent-Type: multipart/form-data; boundary=test\r\nConnection: keep-alive\r\n\r\n2\r\naa\r\n");
	::usleep(10);
	send(socket, "4\r\nbbbb\r\n");
	::usleep(10);
	send(socket, "2\r\ncc\r\n");
	::usleep(10);
	send(socket, "3\r\nddd");
	::usleep(10);
	send(socket, "\r\n2\r");
	::usleep(10);
	send(socket, "\n");
	::usleep(10);
	send(socket, "x");
	::usleep(10);
	send(socket, "xxx\r\n");
	::usleep(10);
	send(socket, "0\r\n\r\n");*/

	/*send(socket, "GET / HTTP/1.1\r\nHost: 0.0");
	::usleep(10);
	send(socket, ".0.0:3000\r\nAccep");
	::usleep(10);
	send(socket, "t-Encoding: gzip\r\nConn");
	::usleep(10);
	send(socket, "ection: keep-alive\r\n");
	::usleep(10);
	send(socket, "\r\n");*/

	::usleep(1000);

	cout << recv(socket) << endl;

	::close(socket);
}

string
recv(const int &fd)
{
	string	packet;
	char	buffer[512];

	for (int b = 0; (b = ::recv(fd, buffer, 511, MSG_DONTWAIT)) > 0;)
	{
		buffer[b] = '\0';
		packet += buffer;
	}
	return (packet);
}

void
send(const int &fd, const string &data)
{
	const int	len = data.size();
	int 		b = 0, r = len, total = 0;

	while (total < len)
	{
		r -= (b = ::send(fd, &data[total], r, MSG_DONTWAIT));
		total += b;
	}
}
