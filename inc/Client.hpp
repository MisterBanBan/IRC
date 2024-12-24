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
        bool is_authenticated;
        std::string ip;
        int port;
        std::set<std::string> channels;
    private:
};

#endif