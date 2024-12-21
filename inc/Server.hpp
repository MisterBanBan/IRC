#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <sys/socket.h> //lib socket
#include <netinet/in.h> //sockaddr_in
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <map>

class Client
{
    public:

    private:
};

class Server
{
    public:
        Server(void);
        Server(const Server &other);
        void initServerSocket(const std::string port, const std::string ip);
        ~Server(void);
        Server &operator=(const Server &other);
    private:
        int _server_fd;
        std::map<int, Client> _clients;
};

#endif