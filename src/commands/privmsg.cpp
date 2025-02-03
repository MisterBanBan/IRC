/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 14:30:37 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::privmsg(std::istringstream &iss, int client_fd) {
	std::string target;
	iss >> target;

	if (!_clients[client_fd].is_authenticated)
	{
		std::string response = "JOIN: You need to be authenticated to do that\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	if (target.empty())
	{
		std::string response = "411 PRIVMSG :No recipient given\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	if (target == _clients[client_fd].nickname)
		return false;

	if (target.size() > 0 && target[0] == '#')
	{
		if (_channels.find(target) == _channels.end())
		{
			std::string response = "403 " + target + "PRIVMSG :No such channel\r\n";
			sendToClient(client_fd, response);
			return true;
		}
		if (_channels[target].getHasKey())
		{
			//peut etre dautre chose a rajouter
			std::string response = "404 " + target + "PRIVMSG :Cannot send to channel\r\n";
			sendToClient(client_fd, response);
			return true;
		}
		std::string msg;
		std::getline(iss, msg);
		while (!msg.empty() && (msg[0] == ' ' || msg[0] == ':'))
		{
			if (msg[0] == ':')
			{
				msg.erase(0, 1);
				if (msg.empty())
				{
					std::string response = "412 PRIVMSG :No text to send\r\n";
					sendToClient(client_fd, response);
					return true;
				}
				broadcastToChannel(target, msg);
				return true;
			}
			msg.erase(0, 1);
		}
		std::string response = "412 PRIVMSG :No text to send\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	int targetFd = getFdByNickname(target);
	if (targetFd < 0)
	{
		std::string response = "401 PRIVMSG :No such nick\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	std::string msg;
	std::getline(iss, msg);
	while (!msg.empty() && (msg[0] == ' ' || msg[0] == ':'))
	{
		if (msg[0] == ':')
		{
			msg.erase(0, 1);
			if (msg.empty())
			{
				std::string response = "412 PRIVMSG :No text to send\r\n";
				sendToClient(client_fd, response);
				return true;
			}
			sendPrivateMessage(client_fd, target, msg);
			return true;
		}
		msg.erase(0, 1);
	}
	std::string response = "412 PRIVMSG :No text to send\r\n";
	sendToClient(client_fd, response);
	return true;
}
