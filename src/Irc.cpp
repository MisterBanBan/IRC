#include <iostream>
#include "Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv <6667> <PASSWORD>\n" << std::endl;
        return 1;
    }
    std::string port = argv[1];
    std::string pass = argv[2];

    Server server;
    server.initServerSocket(port, pass);
    server.run();
    
}