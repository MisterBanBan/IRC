/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:48:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/17 10:48:52 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::user(std::istringstream &iss, int client_fd) {
	std::string user;
	std::string realname;
	iss >> user;
	if (user.empty())
	{
		std::string response = "Error: No user given\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	std::getline(iss, realname);
	if (!realname.empty())
	{
		while (!realname.empty() && (realname[0] == ' ' || realname[0] == ':'))
			realname.erase(0, 1);
	}
	else
		realname = "Unknow";
	if (_clients[client_fd].is_authenticated || !_clients[client_fd].user.empty())
	{
		std::string response = "462 :You are already registered\r\n";
		sendToClient(client_fd, response);
		return false;
	}
	_clients[client_fd].user = user;
	_clients[client_fd].realname = realname;
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
