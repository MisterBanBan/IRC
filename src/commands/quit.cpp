#include "Server.hpp"

bool Server::quit(std::istringstream &iss, int client_fd)
{
    std::string quitMsg;
    getline(iss, quitMsg);
    if (quitMsg.empty())
        quitMsg = "Client Quit";
    std::string nick = getNickname(client_fd);
    std::string msg = ":" + nick + " QUIT :" + quitMsg + "\r\n";
    for (std::set<std::string>::iterator it = _clients[client_fd].channels.begin(); it != _clients[client_fd].channels.end(); ++it)
         broadcastToChannel(*it, msg);
    removeClient(client_fd);
    return true;
}