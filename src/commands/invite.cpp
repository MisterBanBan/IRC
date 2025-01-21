/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:50:09 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/17 10:50:09 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::invite(std::istringstream &iss, int client_fd) {
	//rajouter les invitations
	std::string nickname, channelName;
	iss >> nickname >> channelName;
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
