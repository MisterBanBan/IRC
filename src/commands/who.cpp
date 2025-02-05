/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 10:35:50 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/04 10:35:50 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::who(std::istringstream &iss, int client_fd)
{
    std::string target;
    iss >> target;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED);
		return false;
	}
    if (target.empty())
    {
        sendToClient(client_fd, ERR_NEEDMOREPARAMS("WHO"));
        return false;
    }
    if (target[0] == '#')
    {
        if (_channels.find(target) == _channels.end())
        {
            sendToClient(client_fd, ERR_NOSUCHCHANNEL(target));
            return false;
        }
        std::string response = "352 " + target + " :";
        for (std::set<int>::iterator it = _channels[target].getMembers().begin(); it != _channels[target].getMembers().end(); ++it)
            response += getNickname(*it) + " ";
        response += "\r\n";
        sendToClient(client_fd, response);
        response = "315 " + target + " :End of WHO list\r\n";
        sendToClient(client_fd, response);
        return true;
    }
    else
    {
        std::string response = "WHO :Server-wide WHO not supported\r\n";
        sendToClient(client_fd, response);
        return false;
    }
}