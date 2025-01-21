/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/17 10:49:52 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::privmsg(std::istringstream &iss, int client_fd) {
	std::string target;
	iss >> target;
	if (target.empty())
	{
		std::string response = "411 PRIVMSG :No recipient given\r\n";
		sendToClient(client_fd, response);
		return false;
	}
	std::string msg;
	std::getline(iss, msg);
	while (!msg.empty() && (msg[0] == ' ' || msg[0] == ':'))
		msg.erase(0, 1);
	if (msg.empty())
	{
		std::string response = "412 PRIVMSG :No text to send\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	sendPrivateMessage(client_fd, target, msg);
	return true;
}
