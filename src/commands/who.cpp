/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 10:35:50 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:58:13 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::who(std::istringstream &iss, int client_fd)
{
    std::string target;
    iss >> target;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED(getNickname(client_fd)));
		return;
	}
    if (target.empty())
    {
        sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "WHO"));
        return;
    }
    if (target[0] == '#')
    {
        if (_channels.find(target) == _channels.end())
        {
            sendToClient(client_fd, ERR_NOSUCHCHANNEL(getNickname(client_fd), target));
            return;
        }
        for (std::set<int>::iterator it = _channels[target].getMembers().begin(); it != _channels[target].getMembers().end(); ++it)
		{
			std::string prefix = "";
			if (_channels[target].isOperator(*it))
				prefix = "@";
			sendToClient(client_fd, RPL_WHOREPLY(getNickname(client_fd), getNickname(*it), _channels[target].getName(), getUsername(client_fd), "ip", "localhost", prefix, getRealname(*it)));
		}
        sendToClient(client_fd, RPL_ENDOFWHO(getNickname(client_fd), target));
        return;
    }
    else
    {
        std::string response = "WHO :Server-wide WHO not supported\r\n";
        sendToClient(client_fd, response);
        return;
    }
}