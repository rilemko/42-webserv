/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mconreau <mconreau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/06 20:09:26 by mconreau          #+#    #+#             */
/*   Updated: 2024/07/18 18:33:49 by mconreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http/Request.hpp"

Request::Request(const int &socket) :
	_length(0),
	_remain(0),
	_socket(socket),
	_status(200)
{
}

Request::Request(const Request &src)
{
	*this = src;
}

Request::~Request()
{
}

// 101 : Pending
// 102 : Chunked
// 103 : Bounded

void
Request::recv()
{
	const string	packet = Filesystem::recv(this->_socket);

	//cout << packet << endl;

	if (this->_status == 102)
		return (this->unchunk(packet));
	if (this->_remain > 0)
	{
		const string	bd = packet.substr(0, this->_remain);
		const size_t	bl = packet.size();

		this->_length += bl;
		this->_packet += bd;
		this->_remain -= bl;

		if (this->_remain > 0)
			return (this->setStatus(101));
		else
		{
			if (String::lowercase(this->getHeader("content-type", "")).find("multipart/form-data") == 0)
				this->unbound(this->_packet);
			return (this->setStatus(200));
		}
	}
	if ((this->_packet + packet).find("\r\n\r\n") == string::npos)
		return (this->_packet += packet, this->setStatus(101));
	else
	{
		this->_packet = String::ltrim(this->_packet + packet, "\r\n");
		this->_status = 200;

		size_t			p = this->_packet.find("\r\n\r\n");
		const string	h = this->_packet.substr(0, p + 2);
		stringstream	t(h.substr(0, (p = h.find("\r\n") + 2) - 2));
		string			v;

		if (!(t >> this->_method >> this->_target >> v)) return (this->setStatus(400));
		if (v != "HTTP/1.1") return (this->setStatus(505));
		if (this->_target.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()+,;=") != string::npos) return (this->setStatus(400));
		if (this->_target.size() > 2000) return (this->setStatus(414));

		size_t			d = 0;

		if ((d = this->_target.find('?')) != string::npos)
		{
			this->_params = this->_target.substr(d + 1);
			this->_target = this->_target.substr(0, d);
		}

		if (this->_target.size() > 1 && this->_target.find_first_not_of('/') != string::npos)
			this->_target = Filesystem::normalize(String::rtrim(this->_target, "/"));

		for (size_t e = 0; (e = h.find("\r\n", p)) != string::npos;)
		{
			const string	lne = h.substr(p, e - p);

			if (lne.size() > 8000)
				return (this->setStatus(431));

			if ((d = lne.find(':')) == string::npos)
				return (this->setStatus(400));

			const string 	key = lne.substr(0, d);
			const string	val = lne.substr(d + 1);
			
			this->_header[String::lowercase(String::strim(key, " "))] = String::strim(val, " ");
			p = e + 2;
		}

		if (this->_header.find("host") == this->_header.end())
			return (this->setStatus(400));

		const string	ck = this->getHeader("cookie", "");

		for (size_t p = 0, d = 0; p < ck.size();)
		{
			const string	lne = ck.substr(p, ck.find(';', p) - p);

			if ((d = lne.find('=')) == string::npos)
				return (this->setStatus(400));

			const string	key = lne.substr(0, d);
			const string	val = lne.substr(d + 1);

			this->_cookie[String::strim(key, " ")] = String::strim(val, " ");
			p += lne.size() + 1;
		}

		if (this->_method == "POST" || this->_method == "PUT" || this->_method == "PATCH")
		{
			p += 2;

			if (String::lowercase(this->getHeader("transfer-encoding", "")) == "chunked")
			{
				const string	bd = this->_packet.substr(p);
				return (this->_packet.clear(), this->setStatus(102), this->unchunk(bd));
			}

			size_t			cl;

			if (!(stringstream(this->getHeader("content-length", "0")) >> cl))
				return (this->setStatus(500));

			string			bd = this->_packet.substr(p, cl);
			const size_t	bl = bd.size();

			this->_length = bl;
			this->_packet = bd;

			if (this->_length < cl)
				return (this->_remain = cl - bl, this->setStatus(101));

			if (String::lowercase(this->getHeader("content-type", "")).find("multipart/form-data") == 0)
				return (this->unbound(this->_packet));
		}
		else
			this->_packet.clear();
	}
}

void
Request::unbound(const string &packet)
{
	const string	ct = this->getHeader("content-type", "");

	if (!String::match("multipart/form-data; boundary=*", String::lowercase(ct)))
		return (this->setStatus(400));
	else
	{
		const string	bn = "--" + ct.substr(ct.find('=') + 1);
		const size_t	bs = bn.size();

		for (size_t e = 0, p = bs + 2; (e = packet.find(bn, p)) != string::npos;)
		{
			const string	bp = packet.substr(p, packet.find(bn, p) - p - 2);

			if (String::match("Content-Disposition: form-data; name=\"?*\"; filename=\"?*\"\r\n*\r\n\r\n*", bp))
			{
				const size_t	fs = bp.find("filename=\"") + 10;
				this->_upload[(bp.substr(fs, bp.find("\"", fs) - fs))] = bp.substr(bp.find("\r\n\r\n") + 4);
			}
			p = e + bs + 2;
		}
	}
}

void
Request::unchunk(const string &packet)
{
	const string	pk = this->_packet + packet;

	if (pk.find("0\r\n\r\n") != pk.size() - 5)
		return (this->_packet = pk, this->setStatus(102));
	
	this->_packet.clear();

	for (size_t e = 0, p = 0; (e = pk.find("\r\n", p)) != string::npos;)
		{
			const string	bd = pk.substr(p, e - p);

			if (this->_remain == 0)
			{
				if (bd.find_first_not_of("0123456789abcdef") != string::npos)
					return (this->setStatus(400));

				stringstream	ss;
				unsigned int	cl;

				if (!(ss << hex << bd) || !(ss >> cl))
					return (this->setStatus(500));

				this->_remain += cl;

				if (cl == 0)
				{
					if (String::lowercase(this->getHeader("content-type", "")).find("multipart/form-data") == 0)
						this->unbound(this->_packet);
					return (this->setStatus(200));
				}
			}
			else
			{
				const string	br = bd.substr(0, this->_remain);
				const size_t	bs = br.size();

				this->_length += bs;
				this->_packet += br;
				this->_remain -= bs;
			}
			p = e + 2;
		}
}

string
Request::getCookie(const string &key, const string &other)
{
	return (this->_cookie.find(key) != this->_cookie.end() ? this->_cookie[key] : other);
}

string
Request::getHeader(const string &key, const string &other)
{
	return (this->_header.find(key) != this->_header.end() ? this->_header[key] : other);
}

size_t
Request::getLength() const
{
	return (this->_length);
}

string
Request::getMethod() const
{
	return (this->_method);
}

string
Request::getPacket() const
{
	return (this->_packet);
}

string
Request::getParams() const
{
	return (this->_params);
}

int
Request::getSocket() const
{
	return (this->_socket);
}

size_t
Request::getStatus() const
{
	return (this->_status);
}

string
Request::getTarget() const
{
	return (this->_target);
}

map<string,string>
Request::getUpload() const
{
	return (this->_upload);
}

void
Request::setStatus(const size_t &status)
{
	this->_status = status;
}

Request&
Request::operator=(const Request &rhs)
{
	this->_header = rhs._header;
	this->_length = rhs._length;
	this->_method = rhs._method;
	this->_packet = rhs._packet;
	this->_params = rhs._params;
	this->_socket = rhs._socket;
	this->_status = rhs._status;
	this->_target = rhs._target;
	return (*this);
}
