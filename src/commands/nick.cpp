/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:48:43 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 12:06:42 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

bool Server::nick(std::istringstream & iss, int client_fd)
{
	std::string nickname;
	iss >> nickname;
	if (nickname.empty())
	{
		std::string response = "431 :No nickname given\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	int target_fd = getFdByNickname(nickname);
	if (target_fd > 0)
	{
		if (!_clients[client_fd].is_authenticated)
		{
			std::string response = "NICK :Nickname already taken\r\n";
			sendToClient(client_fd, response);
			removeClient(client_fd);
		}
		else
			sendToClient(client_fd, "NICK :Your nickname already exist\r\n");
		return false;
	}
	_clients[client_fd].nickname = nickname;
	if (!_clients[client_fd].is_authenticated)
	{
		if (!_clients[client_fd].user.empty())
		{
			_clients[client_fd].is_authenticated = true;
			sendToClient(client_fd, ":server 001 " + _clients[client_fd].nickname + " :Welcome!\r\n");
		}
		else
			sendToClient(client_fd, ":server NOTICE * :Please set your USER\r\n");
	}
	std::string response = "Nickname set to " + nickname + "\r\n";
	sendToClient(client_fd, response);
	return true;
}
