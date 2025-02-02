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

	if (!_clients[client_fd].is_authenticated)
	{
		std::string response = "JOIN: You need to be authenticated to do that\r\n";
		sendToClient(client_fd, response);
		return false;
	}
	if (nickname.empty() || channelName.empty())
	{
		std::string response = "461 INVITE :Not enough parameters\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (_channels.find(channelName) == _channels.end())
	{
		std::string response = "403 " + channelName + "INVITE :No such channel\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if(!_channels[channelName].isMember(client_fd))
	{
		std::string response = "442 " + channelName + "INVITE :You're not on that channel\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	int target = getFdByNickname(nickname);
	if (_channels[channelName].isMember(target))
	{
		std::string response = "443 " + channelName + "INVITE :is already on channel\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (_channels[channelName].getInviteOnly() && !_channels[channelName].isOperator(client_fd))
	{
		std::string response = "482 " + channelName + "INVITE :you're not channel operator\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (target < 0)
	{
		std::string response = "401 " + channelName + "INVITE :No such nick\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	std::string response = "341 " + getNickname(client_fd) + " " + nickname + " " + channelName + "\r\n";
	sendToClient(client_fd, response);
	std::string notice = ":" + getNickname(client_fd) + " INVITE " + nickname + " :" + channelName + "\r\n";
	sendToClient(target, notice);
	return true;
}
