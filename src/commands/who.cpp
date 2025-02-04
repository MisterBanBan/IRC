#include "Server.hpp"

bool Server::who(std::istringstream &iss, int client_fd)
{
    std::string target;
    iss >> target;
    if (target.empty())
    {
        std::string response = "461 WHO :Not enough parameters\r\n";
        sendToClient(client_fd, response);
        return false;
    }
    if (target[0] == '#')
    {
        if (_channels.find(target) == _channels.end())
        {
            std::string response = "403 " + target + " :No such channel\r\n";
            sendToClient(client_fd, response);
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
        std::string response = "502 WHO :Server-wide WHO not supported\r\n";
        sendToClient(client_fd, response);
        return false;
    }
}