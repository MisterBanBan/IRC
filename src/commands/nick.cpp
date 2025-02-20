/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:48:43 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:42:06 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::nick(std::istringstream & iss, int client_fd)
{
	std::string nickname;
	iss >> nickname;
	if (nickname.empty())
	{
		sendToClient(client_fd, ERR_NONICKNAMEGIVEN(getNickname(client_fd)));
		return;
	}
	if (nickname[0] == '#' || nickname[0] == ':' || isdigit(nickname[0]))
	{
		sendToClient(client_fd, ERR_ERRONEUSNICKNAME(getNickname(client_fd), nickname));
		return;
	}
	if (nickname.find(" ") != std::string::npos)
	{
		sendToClient(client_fd, ERR_ERRONEUSNICKNAME(getNickname(client_fd), nickname));
		return;
	}
	int target_fd = getFdByNickname(nickname);
	if (target_fd > 0)
	{
		sendToClient(client_fd, ERR_NICKNAMEINUSE(getNickname(client_fd), nickname));
		if (!_clients[client_fd].isAuthenticated())
			authenticate(client_fd);
		return;
	}
	_clients[client_fd].setNickname(nickname);
	std::string response = "Nickname set to " + nickname + "\r\n";
	sendToClient(client_fd, response);

	if (!_clients[client_fd].isAuthenticated())
		authenticate(client_fd);

	return;
}
