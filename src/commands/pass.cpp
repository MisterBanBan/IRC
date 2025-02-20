/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:00 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:42:55 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::pass(std::istringstream &iss, int client_fd)
{
	std::string pass;
	iss >> pass;
	if (pass.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "PASS"));
		return;
	}
	if (_clients[client_fd].isAuthenticated() || _clients[client_fd].getRightPass())
	{
		sendToClient(client_fd, ERR_ALREADYREGISTERED(getNickname(client_fd)));
		return;
	}
	if (!isCorrectPasswordServer(pass))
	{
		sendToClient(client_fd, ERR_PASSWDMISMATCH(getNickname(client_fd)));
		removeClient(client_fd);
		return;
	}

	_clients[client_fd].setRightPass(true);

	std::string response = "NOTICE * :Password accepted\r\n";
	sendToClient(client_fd, response);

	authenticate(client_fd);

	return;
}
