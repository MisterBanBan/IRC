/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 10:23:38 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/19 13:39:17 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ChessBot.hpp"

bool    isARealMove(std::string &msg);

void Server::processBotMessage(int client_fd, std::string &msg)
{
    if (isARealMove(msg)) {
        std::string response = ":Bot PRIVMSG " + getNickname(client_fd) + " :\n";
		this->_ChessTable[client_fd].MakeAPlayerMove(msg);
        this->_ChessTable[client_fd].MovePiece(msg);
        this->_ChessTable[client_fd].ParseBotMove(this->_ChessTable[client_fd].MakeABotMove(), response);
        this->_ChessTable[client_fd].PrintBoard(response);
        sendToClient(client_fd, response);
    }
    else
    {
        if (msg == "Start game")
        {
            this->_ChessTable[client_fd].reset();
            std::string response = ":Bot PRIVMSG " + getNickname(client_fd) + " :\n\n";
            response += "This bot only work if you play legal move, please respect rules of chess !\n";
            (this->_ChessTable[client_fd]).PrintBoard(response);
            sendToClient(client_fd, response);
            return ;
        }
        std::string response = ":Bot PRIVMSG " + getNickname(client_fd) + " :Hi, if you want to start a new game write \"Start game\" or continue a Chess game write your move using the format \"OldpositionNewposition\", exemple :\"e2e4\" will move your pawn on e2 to e4\r\n";
        sendToClient(client_fd, response);
    }
}

bool    isARealMove(std::string &msg)
{
    if (msg.size() >= 4)
    {
        for (std::string::iterator i = msg.begin(); i != msg.end(); i++)
            if (!std::isalnum(*i))
                return (false);
        return (true);
    }
    return (false);
    
    
}

std::string CallAPI(const char* cmd)
{
    std::string result;
    char buffer[128];

    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        throw std::runtime_error("popen() a échoué !");
    }
	
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }

    pclose(pipe);
    return result;
}