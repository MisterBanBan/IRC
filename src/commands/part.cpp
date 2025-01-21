/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:36 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/17 10:49:36 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::part(std::istringstream &iss, int client_fd) {
	std::string channel_name;
	std::string reason;
	if(channel_name.empty())
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
	if (_channels.find(channel_name) == _channels.end())
	{
		std::string response = "403 " + channel_name + " :No such channel\r\n";
		sendToClient(client_fd, response);
		return false;
	}
	if (!_channels[channel_name].isMember(client_fd))
	{
		sendToClient(client_fd, "442 " + channel_name + " :You're not on that channel\r\n");
		return false;
	}
	_channels[channel_name].removeMember(client_fd);
	_clients[client_fd].channels.erase(channel_name);
	std::string part_nick = _clients[client_fd].nickname;
	std::stringstream msg;
	msg << ":" << part_nick
		<< " PART " << channel_name
		<< " "     << part_nick
		<< " :"    << reason
		<< "\r\n";
	broadcastToChannel(channel_name, msg.str());
	sendToClient(client_fd, msg.str());
	return true;
}
