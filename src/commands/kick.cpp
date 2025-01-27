/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:26 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/17 10:49:26 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::kick(std::istringstream &iss, int client_fd) {
	//il faut avoir les droits
	std::string channel_name;
	std::string target_nick;
	std::string reason;
	iss >> channel_name >> target_nick >> reason;
	if(channel_name.empty() || target_nick.empty())
	{
		std::string response = "KICK :Not enough parameters\r\n";
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
		std::string response = "KICK :This channel doesn't exist\r\n";
		sendToClient(client_fd, response);
		return false;
	}
	int target_fd = getFdByNickname(target_nick);
	if (target_fd < 0)
	{
		sendToClient(client_fd, "KICK :No such nickname\r\n");
		return false;
	}
	if (!_channels[channel_name].isMember(target_fd))
	{
		sendToClient(client_fd, "KICK :Target not in channel\r\n");
		return false;
	}
	if (!_channels[channel_name].isMember(client_fd))
	{
		sendToClient(client_fd, "KICK :You are not in this channel\r\n");
		return false;
	}
	_channels[channel_name].removeMember(target_fd);
	_clients[target_fd].channels.erase(channel_name);
	std::string kicker_nick = _clients[client_fd].nickname;
	std::stringstream msg;
	msg << ":" << kicker_nick
		<< " KICK " << channel_name
		<< " "     << target_nick
		<< " :"    << reason
		<< "\r\n";
	broadcastToChannel(channel_name, msg.str());
	sendToClient(target_fd, msg.str());
	return true;
}
