#include "Server.hpp"
#include "Client.hpp"
#include <arpa/inet.h>

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
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY correspond to 0.0.0.0 the server will listen on all available network interfaces convert from host format to network format (little-endian" (the least significant byte is stored first)) big-endian » (high-order byte is stored first) TCP/IP “network byte order”, which is big-endian.
    if (ip == "0.0.0.0")
    {
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } 
    else
    {
        if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
        {
            std::cerr << "Error: Invalid IP address (" << ip << ")" << std::endl;
            close(_server_fd);
            exit(EXIT_FAILURE);
        }
    }
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
    // poll_fds is used to store all file descriptors (sockets) that the server wants to monitor for events.
    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd; //Assigns the server socket file descriptor (_server_fd) to the fd field of the pollfd structure.
    server_pollfd.events = POLLIN; // Sets the POLLIN event type: Indicates that we are monitoring read events.
    server_pollfd.revents = 0; //This field is used by poll() to indicate which events actually occurred.
    poll_fds.push_back(server_pollfd); //This allows the server to detect when a new connection is ready to be accepted
}

void Server::acceptNewClient()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    // _server_fd The file descriptor of the server socket that was initially created with socket() and made to listen with bind() and listen()
    // A pointer to a sockaddr structure where accept() will store the client's address.
    int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) 
    {   // EWOULDBLOCK and EAGAIN are error codes that indicate that the operation should have been blocking, but the socket is in non-blocking mode and no connections were ready to be accepted at the time of the call.
        if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            std::cout << "Error: accept failed (" << strerror(errno) << ")" << std::endl;
        }
        return;
    }
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cout << "Error: F_GETFL failed" << std::endl;
        close(client_fd);
        return;
    }
    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        std::cout << "Error: F_SETFL failed" << std::endl;
        return;
    }
    struct pollfd server_pollfd;
    server_pollfd.fd = client_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    poll_fds.push_back(server_pollfd);

    Client new_client;
    new_client.fd = client_fd;
    new_client.is_authenticated = false;
    _clients[client_fd] = new_client;

    std::cout << "New connexion accepted. FD = " << client_fd << std::endl;

}

void Server::run()
{
    std::cout << "Server is Run" << std::endl;
    while (true)
    {
        //The poll() function monitors a set of file descriptors for events
        //poll_fds.data() to the first element of the poll_fds vector, which contains the pollfd structures describing the file descriptors to monitor
        int ret = poll(poll_fds.data(), poll_fds.size(), -1); // Timeout infini if ret < 0 error
        if(ret < 0)
        {
            std::cout << "Error: poll failde" << std::endl;
            break;
        }
        for (size_t i = 0; i < poll_fds.size(); ++i)
        {
            if (poll_fds[i].revents & POLLIN)
            {
                if (poll_fds[i].fd == _server_fd)
                {
                    acceptNewClient();
                } 
                else 
                {
                    clientData(poll_fds[i].fd);
                }
            }
        }
    }
    close(_server_fd);
}

void Server::clientData(int client_fd)
{
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int receive = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (receive <= 0)
    {
        if (receive == 0)
            std::cout << "Clients disconnected" << client_fd << std::endl;
        else
        {
            std::cout << "Error: recv failed" << std::endl;
            removeClient(client_fd);
            return;
        }
    }

    _clients[client_fd].buffer_in.append(buffer, receive);
    size_t pos;
    while ((pos = _clients[client_fd].buffer_in.find("\r\n")) != std::string::npos)
    {
        std::string command = _clients[client_fd].buffer_in.substr(0, pos);
        _clients[client_fd].buffer_in.erase(0, pos + 2);

        std::cout << "Command receive : " << command << std::endl;

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "NICK")
        {
            std::string nickname;
            iss >> nickname;
            if (nickname.empty())
            {
                std::string response = "Error: No nickname given\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            _clients[client_fd].nickname = nickname;
            std::string response = "Nickname set to " + nickname + "\r\n";
            sendToClient(client_fd, response);
        }
        else if (cmd == "JOIN")
        {
            std::string channel_name;
            iss >> channel_name;
            if (channel_name.empty())
            {
                std::string response = "JOIN :Not enough parameters";
                sendToClient(client_fd, response);
                return;
            }
            if (channels.find(channel_name) == channels.end())
            {
                Channel newChannel;
                newChannel.
            }
        }
        
        else if(cmd == "PART")
        {

        }
    }
}

void Server::sendToClient(int client_fd, const std::string &response)
{
    if (send(client_fd, response.c_str(), response.size(), 0) < 0)
    {
        std::cerr << "Error: send failed to FD " << client_fd << " (" << strerror(errno) << ")" << std::endl;
        removeClient(client_fd);
    }
}

void Server::removeClient(int client_fd)
{
    close(client_fd);
    _clients.erase(client_fd);

    for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it)
    {
        if (it->fd == client_fd)
        {
            poll_fds.erase(it);
            break;
        }
    }
    std::cout << "Client : " << client_fd << " Remove" << std::endl;
}

Server::~Server(void)
{
    for (size_t i = 0; i < poll_fds.size(); ++i)
        close(poll_fds[i].fd);
}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _server_fd = other._server_fd;
        poll_fds = other.poll_fds;
        _clients = other._clients;
    }
    return(*this);
}

