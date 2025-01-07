#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <sys/socket.h> //lib socket
#include <netinet/in.h> //sockaddr_in
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <map>
#include <unistd.h> //close
#include "Client.hpp"
#include "Channel.hpp"
#include <sstream> //istringstream
#include <stdlib.h> //atoi
#include <cstring> //memset
#include <cerrno>



class Server
{
    public:
        Server(void);
        Server(const Server &other);
        void initServerSocket(const std::string port, const std::string);
        void acceptNewClient();
        void clientData(int client_fd);
        void sendToClient(int client_fd, const std::string &reponse);
        void removeClient(int client_fd);
        int getFdByNickname(const std::string target);
        std::string getNickname(int clientFd) const;
        void broadcastToChannel(const std::string &channel_name, const std::string &message);
        void sendPrivateMessage(int client_fd, const std::string &target, const std::string &msg);
        bool isCorrectPasswordServer(const std::string &pass);
        bool isOperator(int client_fd, const std::string &channel);
        size_t getNbUser(int client_fd, const std::string &channel);
        void run();
        ~Server(void);
        Server &operator=(const Server &other);
    private:
        int _server_fd;
        std::vector<struct pollfd> _poll_fds;
        std::map<std::string, Channel> _channels;
        std::map<int, Client> _clients;
        std::string _serverPassword;
};

#endif