#include "Server.hpp"

Server::Server(void) { }

Server::Server(const Server &other)
{
    *this = other;
}

void Server::initServerSocket(const std::string port, const std::string ip)
{
    if (port != "6667")
    {
        std::cout << "<port> is 6667" << std::endl;
        return ;
    }
    int portInt = atoi(port.c_str());
    if (portInt == 0)
    {
        std::cout << "Error: Convert port failed" << std::endl;
        return ;
    }
    //It creates a socket using the protocol IPV4 and a socket orient connexion SOCK_STREAM
    //Concretely, socket() returns a file descriptor (an integer) which identifies this new socket
    //listen on one port
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
    {
        std::cout << "Error: Socket failed" << std::endl;
        exit(1);
    }
    int opt = 1;
    //An option (SO_REUSEADDR) is defined at the socket level (SOL_SOCKET).
    //SO_REUSEADDR allows reuse of address and port immediately after restarting the server
    //opt is used to enable or disable the option 0 disable 1 enable
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR ,&opt, sizeof(opt));
     /*struct sockaddr_in {
    sa_family_t    sin_family;   // Adress Familly (AF_INET for IPv4)
    in_port_t      sin_port;     // The port (in network format)
    struct in_addr sin_addr;     // Adress IPv4
    unsigned char  sin_zero[8];  // Zero-filled, not generally used*/
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); //filling with zeros
    serv_addr.sin_family = AF_INET; //to use familly adress IPV4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY correspond to 0.0.0.0 the server will listen on all available network interfaces convert from host format to network format (little-endian" (the least significant byte is stored first)) big-endian » (high-order byte is stored first) TCP/IP “network byte order”, which is big-endian.
    serv_addr.sin_port = htons(portInt); //the same thing but is to 16 bits
    // bind "attach" the socket to a port on the machine a socket has no address or port defined, and therefore cannot receive connections
    if (bind(_server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Error: bind failed" << std::endl;
        return;
    }
    //SOMAXCONN allows to define the max possible connexion is generally 128 
    if (listen(_server_fd, SOMAXCONN))
    {
        std::cout << "Error: listen failed" << std::endl;
        return;
    }
    int flags = fcntl(_server_fd, F_GETFL, 0); // retrieve the flags of the socket 
    if (flags < 0)
    {
        std::cout << "Error: F_GETFL failed" << std::endl;
        return;
    }
    // manage multiple connections without blocking
    if (fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) < 0) // F_SETFL define a new flags for the socket activate noBLOCK
    {
        std::cout << "Error: F_SETFL failed" << std::endl;
        return;
    }

    std::vector<struct pollfd> poll_fds;
}

Server::~Server(void) { }

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {

    }
    return(*this);
}

