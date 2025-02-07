/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 10:23:38 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/06 16:40:12 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool    isARealMove(std::string msg);

void Server::processBotMessage(int client_fd, const std::string &msg)
{
    if (isARealMove(msg)) {
         std::string response = ":Bot PRIVMSG " + getNickname(client_fd) + " :Pong!\r\n";
         sendToClient(client_fd, response);
    }
    else
    {
        std::string response = ":Bot PRIVMSG " + getNickname(client_fd) + \
            " :Hi, if you want to start or continue a Chess game write your move using the format \"OldpositionNewposition\", exemple :\"e2e4\" will move your pawn on e2 to e4\r\n";
         sendToClient(client_fd, response);
    }
}