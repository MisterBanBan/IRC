/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:00 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 14:05:40 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::pass(std::istringstream &iss, int client_fd) {
	std::string pass;
	iss >> pass;
	if (pass.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS("PASS"));
		return true;
	}
	if (_clients[client_fd].isAuthenticated() || _clients[client_fd].getRightPass())
	{
		sendToClient(client_fd, "462 PASS :You may not reregister\r\n");
		return true;
	}
	if (!isCorrectPasswordServer(pass))
	{
		sendToClient(client_fd, ERR_PASSWDMISMATCH);
		removeClient(client_fd);
		return false;
	}

	_clients[client_fd].setRightPass(true);

	std::string response = "NOTICE * :Password accepted\r\n";
	sendToClient(client_fd, response);

	authenticate(client_fd);

	return true;
}
