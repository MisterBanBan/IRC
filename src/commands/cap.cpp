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
    sendToClient(client_fd, "421 CAP :Unknown subcommand\r\n");
    return false;
}