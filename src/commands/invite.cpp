/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:50:09 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 14:30:37 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::invite(std::istringstream &iss, int client_fd)
{
	//rajouter les invitations
	std::string nickname, channelName;
	iss >> nickname >> channelName;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED);
		return false;
	}
	if (nickname.empty() || channelName.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS("INVITE"));
		return true;
	}
	if (_channels.find(channelName) == _channels.end())
	{
		sendToClient(client_fd, ERR_NOSUCHCHANNEL(channelName));
		return true;
	}
	if(!_channels[channelName].isMember(client_fd))
	{
		sendToClient(client_fd, ERR_NOTONCHANNEL(channelName));
		return true;
	}
	int target = getFdByNickname(nickname);
	if (_channels[channelName].isMember(target))
	{
		sendToClient(client_fd, ERR_USERONCHANNEL(nickname, channelName));
		return true;
	}
	if (_channels[channelName].getInviteOnly() && !_channels[channelName].isOperator(client_fd))
	{
		sendToClient(client_fd, ERR_CHANOPRIVSNEEDED(channelName));
		return true;
	}
	if (target < 0)
	{
		sendToClient(client_fd, ERR_NOSUCHNICK(channelName));
		return true;
	}
	sendToClient(client_fd, RPL_INVITING(nickname, channelName));
	std::string notice = ":" + getNickname(client_fd) + " INVITE " + nickname + " :" + channelName + "\r\n";
	sendToClient(target, notice);
	return true;
}
