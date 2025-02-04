/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 10:23:38 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/04 10:23:38 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::processBotMessage(int client_fd, const std::string &msg)
{
    if (msg == "!ping") {
         std::string response = ":Bot PRIVMSG " + getNickname(client_fd) + " :Pong!\r\n";
         sendToClient(client_fd, response);
    }
}