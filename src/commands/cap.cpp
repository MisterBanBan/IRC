/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 10:23:38 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/04 10:23:38 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::cap(std::istringstream &iss, int client_fd)
{
    std::string command;
    iss >> command;
    if (command == "LS")
    {
        std::string response = ":server CAP * LS :multi-prefix sasl\r\n";
        sendToClient(client_fd, response);
        return true;   
    }
    else if (command == "REQ")
    {
        std::string req;
        std::getline(iss, req);
        sendToClient(client_fd, ":server CAP * ACK" + req + "\r\n");
        return true;
    }
    else if (command == "END")
    {
        return true;
    }
	std::stringstream ss;
	ss << "CAP " << command;
    sendToClient(client_fd, ERR_UNKNOWNCOMMAND(getNickname(client_fd), ss.str()));
    return false;
}