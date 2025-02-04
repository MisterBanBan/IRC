/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:48:43 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 14:05:40 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::nick(std::istringstream & iss, int client_fd)
{
	std::string nickname;
	iss >> nickname;
	if (nickname.empty())
	{
		std::string response = "431 :No nickname given\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	int target_fd = getFdByNickname(nickname);
	if (target_fd > 0)
	{
		std::string response = "433 NICK :Nickname is already in use\r\n";
		sendToClient(client_fd, response);
		if (!_clients[client_fd].isAuthenticated())
			removeClient(client_fd);
		return true;
	}
	_clients[client_fd].setNickname(nickname);
	std::string response = "Nickname set to " + nickname + "\r\n";
	sendToClient(client_fd, response);

	if (!_clients[client_fd].isAuthenticated())
		authenticate(client_fd);

	return true;
}
