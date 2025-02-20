/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:43:45 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::privmsg(std::istringstream &iss, int client_fd) {
	std::string target;
	iss >> target;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_ALREADYREGISTERED(getNickname(client_fd)));
		return;
	}

	if (target.empty())
	{
		sendToClient(client_fd, ERR_NORECIPIENT(getNickname(client_fd)));
		return;
	}

	if (target == "Bot")
	{
		std::string botMsg;
        std::getline(iss, botMsg);
        while (!botMsg.empty() && (botMsg[0] == ' ' || botMsg[0] == ':')) {
            botMsg.erase(0, 1);
        }
        processBotMessage(client_fd, botMsg);
        return;
	}

	if (target == _clients[client_fd].getNickname())
		return;

	if (target.size() > 0 && target[0] == '#')
	{
		if (_channels.find(target) == _channels.end())
		{
			sendToClient(client_fd, ERR_NOSUCHCHANNEL(getNickname(client_fd), target));
			return;
		}

		if (!_channels[target].isMember(client_fd))
		{
			sendToClient(client_fd, ERR_NOTONCHANNEL(getNickname(client_fd), target));
			return;
		}

		std::string msg;
		std::getline(iss, msg);
		while (!msg.empty() && (msg[0] == ' ' || msg[0] == ':'))
		{
			if (msg[0] == ':')
			{
				if (msg.empty())
				{
					sendToClient(client_fd, ERR_NOTEXTTOSEND(getNickname(client_fd)));
					return;
				}
				std::stringstream response;
				response << ":" << _clients[client_fd].getNickname() << " PRIVMSG " << target << " " << msg << "\r\n";
				broadcastToChannel(target, response.str(), client_fd);
				return;
			}
			msg.erase(0, 1);
		}
		sendToClient(client_fd, ERR_NOTEXTTOSEND(getNickname(client_fd)));
		return;
	}
	int targetFd = getFdByNickname(target);
	if (targetFd < 0)
	{
		sendToClient(client_fd, ERR_NOSUCHNICK(getNickname(client_fd), target));
		return;
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
				sendToClient(client_fd, ERR_NOTEXTTOSEND(getNickname(client_fd)));
				return;
			}
			sendPrivateMessage(client_fd, target, msg);
			return;
		}
		msg.erase(0, 1);
	}
	sendToClient(client_fd, ERR_NOTEXTTOSEND(getNickname(client_fd)));
	return;
}
