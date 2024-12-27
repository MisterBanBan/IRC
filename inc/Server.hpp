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



class Server
{
    public:
        Server(void);
        Server(const Server &other);
        void initServerSocket(const std::string port, const std::string ip);
        void acceptNewClient();
        void clientData(int client_fd);
        void sendToClient(int client_fd, const std::string &reponse);
        void removeClient(int client_fd);
        int getFdByNickname(const std::string target);
        std::set<int> getMembers() const { return _members; }
        void broadcastToChannel(const std::string &channel_name, const std::string &message);
        void run();
        ~Server(void);
        Server &operator=(const Server &other);
    private:
        int _server_fd;
        std::vector<struct pollfd> _poll_fds;
        std::map<std::string, Channel> _channels;
        std::map<int, Client> _clients;
};

#endif