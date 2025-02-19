/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:48:52 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/07 17:35:32 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::user(std::istringstream &iss, int client_fd) {
	std::string user;
	std::string realname;
	iss >> user;
	if (user.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "USER"));
		return true;
	}
	if (_clients[client_fd].isAuthenticated() || !_clients[client_fd].getUsername().empty())
	{
		sendToClient(client_fd, ERR_ALREADYREGISTERED(getNickname(client_fd)));
		return false;
	}

	std::getline(iss, realname);

	std::vector<std::string>	args	=	split(realname, ' ');

	if (args.size() < 3)
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "USER"));
		return true;
	}

	for (size_t i = 0; i < 3; i++) {
		if ((i == 0 && args[i] != "0") || (i == 1 && args[i] != "*") || (i == 2 && args[i][0] != ':')) {
			sendToClient(client_fd, ERR_UNKNOWNCOMMAND(getNickname(client_fd), "USER " + realname));
			authenticate(client_fd);
			return false;
		}
	}

	realname.clear();
	args[2].erase(0, 1);
	for (size_t i = 2; i < args.size(); i++)
		realname += args[i] + " ";
	realname.erase(realname.size() - 1, 1);

	if (realname.empty())
		_clients[client_fd].setRealname("Unknown");
	else
		_clients[client_fd].setRealname(realname);

	_clients[client_fd].setUsername(user);

	std::stringstream ss;
	ss << "USER command from FD " << client_fd
	   << " => username: " << _clients[client_fd].getUsername()
	   << ", realname: " << _clients[client_fd].getRealname();
	std::string response = ss.str() + "\r\n";
	sendToClient(client_fd, response);

	authenticate(client_fd);

	return true;
}
