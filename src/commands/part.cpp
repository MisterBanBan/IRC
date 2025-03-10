/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:36 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:42:31 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::part(std::istringstream &iss, int client_fd) {
	std::string channel_str;
	std::string reason;
	iss >> channel_str;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED(getNickname(client_fd)));
		return;
	}

	if(channel_str.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "PART"));
		return;
	}
	std::getline(iss, reason);
	if (!reason.empty())
	{
		if (reason[0] == ' ')
			reason.erase(0,1);
	}
	if (reason.empty())
		reason = "No reason";
	std::vector<std::string> channels = split(channel_str, ',');
	for (size_t i = 0; i < channels.size(); i++)
	{
		std::string channel_name = channels[i];
		if (_channels.find(channel_name) == _channels.end())
		{
			sendToClient(client_fd, ERR_NOSUCHCHANNEL(getNickname(client_fd), channel_name));
			return;
		}
		if (!_channels[channel_name].isMember(client_fd))
		{
			sendToClient(client_fd, ERR_NOTONCHANNEL(getNickname(client_fd), channel_name));
			return;
		}
		broadcastToChannel(channel_name, PART(_clients[client_fd].getNickname(), channel_name, reason), -1);
		leaveChannel(channel_name, client_fd);
	}
	return;
}
