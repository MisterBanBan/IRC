/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:36 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 14:30:37 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::part(std::istringstream &iss, int client_fd) {
	std::string channel_str;
	std::string reason;
	iss >> channel_str;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED);
		return false;
	}

	if(channel_str.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS("PART"));
		return false;
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
			sendToClient(client_fd, ERR_NOSUCHCHANNEL(channel_name));
			return false;
		}
		if (!_channels[channel_name].isMember(client_fd))
		{
			sendToClient(client_fd, ERR_NOTONCHANNEL(channel_name));
			return false;
		}
		broadcastToChannel(channel_name, PART(_clients[client_fd].getNickname(), channel_name, reason), -1);
		_channels[channel_name].removeMember(client_fd);
		_clients[client_fd].getChannels().erase(channel_name);
		if (_channels[channel_name].getMembers().empty())
			_channels.erase(channel_name);
	}
	
	return true;
}
