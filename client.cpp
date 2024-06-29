/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/22 20:09:54 by mconreau          #+#    #+#             */
/*   Updated: 2024/06/28 21:42:53 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>

using namespace std;

int
main(void)
{
	const int 	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	hostent*	server = ::gethostbyname("0.0.0.0");
	string		packet;

	sockaddr_in	addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = ::htons(3000);
    ::bcopy(server->h_addr, &addr.sin_addr.s_addr, static_cast<size_t>(server->h_length));

	if (::connect(socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1)
		return (perror("send"), 1);

	packet = "GET / HTTP/1.1\r\nHost: 0.0.0.0\r\nTransfer-Encoding: chunked\r\nConnection: keep-alive\r\n\r\n2\r\naa\r\n4\r\nbbbb\r\n";
	::send(socket, packet.c_str(), packet.size(), 0);

	::usleep(1);

	//packet = "2\r\naa\r\n";
	//::send(socket, packet.c_str(), packet.size(), 0);
	
	//::usleep(1);

	//packet = "4\r\nbbbb\r\n";
	//::send(socket, packet.c_str(), packet.size(), 0);

	//::usleep(1);

	packet = "2\r\ncc\r\n";
	::send(socket, packet.c_str(), packet.size(), 0);

	::usleep(1);

	packet = "3\r\nddd\r\n";
	::send(socket, packet.c_str(), packet.size(), 0);

	::usleep(1);

	packet = "2\r\nxxxxx\r\n";
	::send(socket, packet.c_str(), packet.size(), 0);
	
	::usleep(1);

	packet = "0\r\n\r\n";
	::send(socket, packet.c_str(), packet.size(), 0);

	::usleep(1);

	char	buffer[10000];
	::recv(socket, buffer, sizeof(buffer), 0);

	cout << buffer << endl;

	::close(socket);
}
