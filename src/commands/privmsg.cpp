/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 14:45:06 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::privmsg(std::istringstream &iss, int client_fd) {
	std::string target;
	iss >> target;

	if (!_clients[client_fd].isAuthenticated()) {
		sendToClient(client_fd, ERR_ALREADYREGISTERED(getNickname(client_fd)));
		return;
	}

	if (target.empty()) {
		sendToClient(client_fd, ERR_NORECIPIENT(getNickname(client_fd)));
		return;
	}

	if (target == "Bot") {
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

	if (target[0] == '#' && _channels.find(target) == _channels.end())
	{
		sendToClient(client_fd, ERR_NOSUCHCHANNEL(_clients[client_fd].getNickname(), target));
		return;
	}

	if (target[0] != '#' && getFdByNickname(target) < 0)
	{
		sendToClient(client_fd, ERR_NOSUCHNICK(_clients[client_fd].getNickname(), target));
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

void Server::sendPrivateMessage(int client_fd, const std::string &target, const std::string &msg)
{
	if (target[0] == '#')
	{
		Channel &chan = _channels[target];

		if (!chan.isMember(client_fd))
		{
			sendToClient(client_fd, ERR_NOTONCHANNEL(_clients[client_fd].getNickname(), target));
			return;
		}
		std::string nick_sender = getNickname(client_fd);
		std::string msg_formatted = ":" + nick_sender + " PRIVMSG " + target + " :" + msg + "\r\n";
		broadcastToChannel(target, msg_formatted, client_fd);
		return ;
	}
	else
	{
		int target_fd = getFdByNickname(target);
		std::string nick_sender = getNickname(client_fd);
		std::string msg_formatted = ":" + nick_sender + " PRIVMSG " + target + " :" + msg + "\r\n";
		sendToClient(target_fd, msg_formatted);
		return;
	}
}
