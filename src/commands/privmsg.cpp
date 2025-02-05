/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/04 12:40:20 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::privmsg(std::istringstream &iss, int client_fd) {
	std::string target;
	iss >> target;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_ALREADYREGISTERED);
		return false;
	}

	if (target.empty())
	{
		sendToClient(client_fd, ERR_NORECIPIENT);
		return false;
	}

	if (target == "Bot")
	{
		std::string botMsg;
        std::getline(iss, botMsg);
        while (!botMsg.empty() && (botMsg[0] == ' ' || botMsg[0] == ':')) {
            botMsg.erase(0, 1);
        }
        processBotMessage(client_fd, botMsg);
        return true;
	}

	if (target == _clients[client_fd].getNickname())
		return false;

	if (target.size() > 0 && target[0] == '#')
	{
		if (_channels.find(target) == _channels.end())
		{
			sendToClient(client_fd, ERR_NOSUCHCHANNEL(target));
			return true;
		}

		if (!_channels[target].isMember(client_fd))
		{
			sendToClient(client_fd, ERR_NOTONCHANNEL(target));
			return true;
		}

		std::string msg;
		std::getline(iss, msg);
		while (!msg.empty() && (msg[0] == ' ' || msg[0] == ':'))
		{
			if (msg[0] == ':')
			{
				if (msg.empty())
				{
					sendToClient(client_fd, ERR_NOTEXTTOSEND);
					return true;
				}
				std::stringstream response;
				response << ":" << _clients[client_fd].getNickname() << " PRIVMSG " << target << " " << msg << "\r\n";
				broadcastToChannel(target, response.str(), client_fd);
				return true;
			}
			msg.erase(0, 1);
		}
		sendToClient(client_fd, ERR_NOTEXTTOSEND);
		return true;
	}
	int targetFd = getFdByNickname(target);
	if (targetFd < 0)
	{
		sendToClient(client_fd, ERR_NOSUCHNICK(target));
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
				sendToClient(client_fd, ERR_NOTEXTTOSEND);
				return true;
			}
			sendPrivateMessage(client_fd, target, msg);
			return true;
		}
		msg.erase(0, 1);
	}
	sendToClient(client_fd, ERR_NOTEXTTOSEND);
	return true;
}
