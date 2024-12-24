#include <iostream>
#include "Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv <127.0.0.1> <6667>\n" << std::endl;
        return 1;
    }
    std::string port = argv[2];
    std::string ip = argv[1];

    Server server;
    server.initServerSocket(port, ip);
    server.run();
    
}