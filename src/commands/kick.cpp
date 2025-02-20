/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:26 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 13:28:45 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::kick(std::istringstream &iss, int client_fd) {
	std::string channel_name;
	std::string target_nick;
	std::string reason;
	iss >> channel_name >> target_nick >> reason;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED(getNickname(client_fd)));
		return;
	}

	if(channel_name.empty() || target_nick.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "KICK"));
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
	if (_channels.find(channel_name) == _channels.end())
	{
		sendToClient(client_fd, ERR_NOSUCHCHANNEL(getNickname(client_fd), channel_name));
		return;
	}

	Channel &chan = _channels[channel_name];
	if (!chan.isOperator(client_fd))
	{
		sendToClient(client_fd, ERR_CHANOPRIVSNEEDED(getNickname(client_fd), channel_name));
		return;
	}
	int target_fd = getFdByNickname(target_nick);
	if (target_fd < 0)
	{
		sendToClient(client_fd, ERR_NOSUCHNICK(getNickname(client_fd), target_nick));
		return;
	}
	if (!chan.isMember(target_fd))
	{
		sendToClient(client_fd, ERR_USERNOTINCHANNEL(getNickname(client_fd), target_nick, channel_name));
		return;
	}
	if (!chan.isMember(client_fd))
	{
		sendToClient(client_fd, ERR_NOTONCHANNEL(getNickname(client_fd), channel_name));
		return;
	}
	chan.removeMember(target_fd);
	_clients[target_fd].getChannels().erase(channel_name);
	std::string kicker_nick = _clients[client_fd].getNickname();
	broadcastToChannel(channel_name, KICK(kicker_nick, channel_name, target_nick, reason), -1);
	sendToClient(target_fd, KICK(kicker_nick, channel_name, target_nick, reason));
	if (chan.getMembers().empty())
		_channels.erase(channel_name);
	return;
}
