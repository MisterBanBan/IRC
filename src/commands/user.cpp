/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:48:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/31 18:46:36 by mtbanban         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::user(std::istringstream &iss, int client_fd) {
	std::string user;
	std::string realname;
	iss >> user;
	if (user.empty())
	{
		std::string response = "461 USER :Not enough parameters\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (_clients[client_fd].passWord == false)
	{
		std::string response = "NICK :Please enter a password\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (_clients[client_fd].is_authenticated || !_clients[client_fd].user.empty())
	{
		std::string response = "462 USER :You may not register\r\n";
		sendToClient(client_fd, response);
		return false;
	}
	std::getline(iss, realname);
	if (!realname.empty())
	{
		while (!realname.empty() && (realname[0] == ' ' || realname[0] == ':'))
		{
			if (realname[0] == ':')
			{
				realname.erase(0, 1);
				if (!realname.empty())
					_clients[client_fd].realname = realname;
			}
			realname.erase(0, 1);
		}
	}
	else
			_clients[client_fd].realname = "Unknown";
	_clients[client_fd].user = user;
	if (!_clients[client_fd].nickname.empty())
	{
		_clients[client_fd].is_authenticated = true;
		sendToClient(client_fd, ":server 001 " + _clients[client_fd].nickname + " :Welcome!\r\n");
	}
	else
		sendToClient(client_fd, ":server NOTICE * :Please set your NICK\r\n");
	std::stringstream ss;
	ss << "USER command from FD " << client_fd
	   << " => username: " << _clients[client_fd].user
	   << ", realname: " << _clients[client_fd].realname;
	std::string response = ss.str() + "\r\n";
	sendToClient(client_fd, response);
	return true;
}
