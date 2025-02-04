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

	if (!_clients[client_fd].is_authenticated)
	{
		std::string response = "JOIN: You need to be authenticated to do that\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	if(channel_str.empty())
	{
		std::string response = "461 PART :Not enough parameters\r\n";
		sendToClient(client_fd, response);
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
			std::string response = "403 " + channel_name + " PART: No such channel\r\n";
			sendToClient(client_fd, response);
			return false;
		}
		if (!_channels[channel_name].isMember(client_fd))
		{
			sendToClient(client_fd, "442 " + channel_name + " PART: You're not on that channel\r\n");
			return false;
		}
		std::string part_nick = _clients[client_fd].nickname;
		std::stringstream msg;
		msg << ":" << part_nick
			<< " PART " << channel_name
			<< " "     << part_nick
			<< " :"    << reason
			<< "\r\n";
		broadcastToChannel(channel_name, msg.str(), -1);
		_channels[channel_name].removeMember(client_fd);
		_clients[client_fd].channels.erase(channel_name);
		if (_channels[channel_name].getMembers().empty())
			_channels.erase(channel_name);
	}
	
	return true;
}
