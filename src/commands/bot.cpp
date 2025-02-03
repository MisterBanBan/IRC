#include "Server.hpp"

void Server::processBotMessage(int client_fd, const std::string &msg)
{
    if (msg == "!ping") {
         std::string response = ":Bot PRIVMSG " + getNickname(client_fd) + " :Pong!\r\n";
         sendToClient(client_fd, response);
    }
}