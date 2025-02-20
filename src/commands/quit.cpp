/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:38:11 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:44:48 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::quit(std::istringstream &iss, int client_fd)
{
    std::string quitMsg;
    getline(iss, quitMsg);
    if (quitMsg.empty())
        quitMsg = "Client Quit";
    std::string nick = getNickname(client_fd);
    std::string msg = ":" + nick + " QUIT :" + quitMsg + "\r\n";
    for (std::set<std::string>::iterator it = _clients[client_fd].getChannels().begin(); it != _clients[client_fd].getChannels().end(); ++it)
         broadcastToChannel(*it, msg, -1);
    removeClient(client_fd);
    return;
}