/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:26 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 16:02:48 by mtbanban         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::kick(std::istringstream &iss, int client_fd) {
	//il faut avoir les droits
	std::string channel_name;
	std::string target_nick;
	std::string reason;
	iss >> channel_name >> target_nick >> reason;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED);
		return false;
	}

	if(channel_name.empty() || target_nick.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS("KICK"));
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
		sendToClient(client_fd, ERR_NOSUCHCHANNEL(channel_name));
		return false;
	}
	if (!_channels[channel_name].isOperator(client_fd))
	{
		sendToClient(client_fd, ERR_CHANOPRIVSNEEDED(channel_name));
		return false;
	}
	int target_fd = getFdByNickname(target_nick);
	if (target_fd < 0)
	{
		sendToClient(client_fd, ERR_NOSUCHNICK(target_nick));
		return false;
	}
	if (!_channels[channel_name].isMember(target_fd))
	{
		sendToClient(client_fd, ERR_USERNOTINCHANNEL(target_nick, channel_name));
		return false;
	}
	if (!_channels[channel_name].isMember(client_fd))
	{
		sendToClient(client_fd, ERR_NOTONCHANNEL(channel_name));
		return false;
	}
	_channels[channel_name].removeMember(target_fd);
	_clients[target_fd].getChannels().erase(channel_name);
	std::string kicker_nick = _clients[client_fd].getNickname();
	broadcastToChannel(channel_name, KICK(kicker_nick, channel_name, target_nick, reason), -1);
	sendToClient(target_fd, KICK(kicker_nick, channel_name, target_nick, reason));
	if (_channels[channel_name].getMembers().empty())
		_channels.erase(channel_name);
	return true;
}
