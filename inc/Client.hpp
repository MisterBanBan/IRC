#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <set>

class Client
{
    public:
        int fd;
        std::string buffer_in;
        std::string buffer_out;
        std::string nickname;
        std::string user;
        std::string realname;
        std::string pass;
        bool is_authenticated;
        std::string ip;
        int port;
        std::set<std::string> channels;
        
        std::string getNickname() const { return nickname; }
    private:
};

#endif