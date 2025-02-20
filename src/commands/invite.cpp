/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:50:09 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:39:36 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::invite(std::istringstream &iss, int client_fd)
{
	std::string nickname, channelName;
	iss >> nickname >> channelName;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED(getNickname(client_fd)));
		return;
	}
	if (nickname.empty() || channelName.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "INVITE"));
		return;
	}
	if (_channels.find(channelName) == _channels.end())
	{
		sendToClient(client_fd, ERR_NOSUCHCHANNEL(getNickname(client_fd), channelName));
		return;
	}
	if(!_channels[channelName].isMember(client_fd))
	{
		sendToClient(client_fd, ERR_NOTONCHANNEL(getNickname(client_fd), channelName));
		return;
	}
	int target = getFdByNickname(nickname);
	if (_channels[channelName].isMember(target))
	{
		sendToClient(client_fd, ERR_USERONCHANNEL(getNickname(client_fd), nickname, channelName));
		return;
	}
	if (_channels[channelName].isInviteOnly() && !_channels[channelName].isOperator(client_fd))
	{
		sendToClient(client_fd, ERR_CHANOPRIVSNEEDED(getNickname(client_fd), channelName));
		return;
	}
	if (target < 0)
	{
		sendToClient(client_fd, ERR_NOSUCHNICK(getNickname(client_fd), nickname));
		return;
	}
	_channels[channelName].inviteUser(target);
	sendToClient(client_fd, RPL_INVITING(getNickname(client_fd), nickname, channelName));
	sendToClient(target, INVITE(getNickname(client_fd), nickname, channelName));
	return;
}
