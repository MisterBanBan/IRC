/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 12:43:53 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/12 13:03:41 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include <arpa/inet.h>
#include <sstream>

Server::Server(void) : _running(true) { }

Server::Server(const Server &other)
{
    *this = other;
}

void Server::stop()
{
    _running = false;
}


int	Server::initServerSocket(const std::string & port, const std::string & pass)
{
    if (port != "6667")
    {
        std::cout << "<port> is 6667" << std::endl;
        return 1;
    }
	char *end;
    int portInt = strtol(port.c_str(), &end, 10);
    if (*end != '\0')
    {
        std::cout << "Error: Convert port failed" << std::endl;
        return 1;
    }
    if (pass.empty())
    {
        std::cout << "Error: Password is empty" << std::endl;
        return 1;
    }
    else
        this->_serverPassword = pass;
    //It creates a socket using the protocol IPV4 and a socket orient connexion SOCK_STREAM
    //Concretely, socket() returns a file descriptor (an integer) which identifies this new socket
    //listen on one port
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
    {
        std::cout << "Error: Socket failed (" << errno << ")" << std::endl;
        return 1;
    }
    int opt = 1;
    //An option (SO_REUSEADDR) is defined at the socket level (SOL_SOCKET).
    //SO_REUSEADDR allows reuse of address and port immediately after restarting the server
    //opt is used to enable or disable the option 0 disable 1 enable
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR ,&opt, sizeof(opt)) == -1)
	{
		std::cout << "Error: " << errno << std::endl;
		return 1;
	}
     /*struct sockaddr_in {
    sa_family_t    sin_family;   // Adress Familly (AF_INET for IPv4)
    in_port_t      sin_port;     // The port (in network format)
    struct in_addr sin_addr;     // Adress IPv4
    unsigned char  sin_zero[8];  // Zero-filled, not generally used*/
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); //filling with zeros
    serv_addr.sin_family = AF_INET; //to use familly adress IPV4
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY correspond to 0.0.0.0 the server will listen on all available network interfaces convert from host format to network format (little-endian" (the least significant byte is stored first)) big-endian » (high-order byte is stored first) TCP/IP “network byte order”, which is big-endian.
    /*if (ip == "0.0.0.0")
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
    }*/
    serv_addr.sin_port = htons(portInt); //the same thing but is to 16 bits
    // bind "attach" the socket to a port on the machine a socket has no address or port defined, and therefore cannot receive connections
    if (bind(_server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Error: bind failed (" << strerror(errno) << ")" << std::endl;
        return 1;
    }
    //SOMAXCONN allows to define the max possible connexion is generally 128 
    if (listen(_server_fd, SOMAXCONN))
    {
        std::cout << "Error: listen failed" << std::endl;
        return 1;
    }
    int flags = fcntl(_server_fd, F_GETFL, 0); // retrieve the flags of the socket 
    if (flags < 0)
    {
        std::cout << "Error: F_GETFL failed" << std::endl;
        return 1;
    }
    // manage multiple connections without blocking
    if (fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) < 0) // F_SETFL define a new flags for the socket activate noBLOCK
    {
        std::cout << "Error: F_SETFL failed" << std::endl;
        return 1;
    }
    // _poll_fds is used to store all file descriptors (sockets) that the server wants to monitor for events.
    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd; //Assigns the server socket file descriptor (_server_fd) to the fd field of the pollfd structure.
    server_pollfd.events = POLLIN; // Sets the POLLIN event type: Indicates that we are monitoring read events.
    server_pollfd.revents = 0; //This field is used by poll() to indicate which events actually occurred.
    _poll_fds.push_back(server_pollfd); //This allows the server to detect when a new connection is ready to be accepted
	return 0;
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
    _poll_fds.push_back(server_pollfd);

    Client new_client(client_fd);
    _clients[client_fd] = new_client;

    std::cout << "New connexion accepted. FD = " << client_fd << std::endl;
	authenticate(client_fd);

}


void Server::run()
{
    std::cout << "Server is running" << std::endl;
    while (_running)
    {
        //The poll() function monitors a set of file descriptors for events
        //_poll_fds.data() to the first element of the _poll_fds vector, which contains the pollfd structures describing the file descriptors to monitor
        int ret = poll(_poll_fds.data(), _poll_fds.size(), -1); // Timeout infini if ret < 0 error
        if(ret < 0)
        {
            std::cout << "Error: poll failde" << std::endl;
            break;
        }
        for (size_t i = 0; i < _poll_fds.size();)
        {
            int fd = _poll_fds[i].fd;
            short revents = _poll_fds[i].revents;
            // If errors or a disconnection (POLLERR, POLLHUP, POLLNVAL) are detected, the client is removed (which also removes the _poll_fds descriptor).
            if (revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                //As _poll_fds has been modified, we do not do any incrementation here
                removeClient(fd);
                continue;
            }
            if (revents & POLLIN)
            {
                if (fd == _server_fd)
                    acceptNewClient();
                else 
                    clientData(fd);
            }
            //If the write event (POLLOUT) is detected, we call the handleWriteEvent function to send the pending data to the buffer.
            if (revents & POLLOUT)
                handleWriteEvent(fd);
            ++i;
        }
    }
    close(_server_fd);
}

void Server::clientData(int client_fd)
{
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    ssize_t receive = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (receive <= 0)
    {
        if (receive == 0)
        {
            std::cout << "Clients disconnected" << client_fd << std::endl;
            removeClient(client_fd);
            return;
        }
        else
        {
            std::cout << "Error: recv failed" << std::endl;
            removeClient(client_fd);
            return;
        }
    }

    _clients[client_fd].getBufferIn().append(buffer, receive);

	size_t pos;

	while ((pos = _clients[client_fd].getBufferIn().find("\n")) != std::string::npos)
	{
		std::string command = _clients[client_fd].getBufferIn().substr(0, pos);
		_clients[client_fd].getBufferIn().erase(0, pos + 1);

		if (command.empty())
			continue;

        std::cout << "Command received : " << command << std::endl;

		if (command.at(command.size() - 1) == 13)
			command.erase(command.size() - 1);

        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "NICK")
        {
			if (nick(iss, client_fd))
				continue;
			return;
        }
        else if (cmd == "USER")
        {
			if (user(iss, client_fd))
				continue;
			return;
        }
        else if (cmd == "PASS")
        {
          if (pass(iss, client_fd))
			  continue;
		  return;
        }
        else if (cmd == "JOIN")
        {
           if (join(iss, client_fd))
			   continue;
			return;
        }
        else if (cmd == "KICK")
        {
			if (kick(iss, client_fd))
				continue;
			return;
        }
        else if(cmd == "PART")
        {
           if (part(iss, client_fd))
			   continue;
			return;
        }
        else if (cmd == "PRIVMSG")
        {
            if (privmsg(iss, client_fd))
				continue;
			return;
        }
        else if (cmd == "TOPIC")
        {
			if (topic(iss, client_fd))
				continue;
			return;
        }
        else if (cmd == "INVITE")
        {
            if (invite(iss, client_fd))
				continue;
			return;
        }
        else if (cmd == "MODE")
        {
			if (mode(iss, client_fd))
				continue;
			return;
        }
        if (cmd == "CAP")
        {
            if (cap(iss, client_fd))
                continue;
            return;
        }
        else if (cmd == "WHO")
        {
            if (who(iss, client_fd))
                continue;
            return;
        }
        else if (cmd == "QUIT")
        {
            if (quit(iss, client_fd))
                continue;
            return;
        }
        else
            sendToClient(client_fd, ERR_UNKNOWNCOMMAND(cmd));
    }
}

int Server::getNbUser(int clients_fd, const std::string &channel)
{
    int i = 0;
	(void)clients_fd;
    for (std::set<int>::iterator it = _channels[channel].getMembers().begin(); it != _channels[channel].getMembers().end(); ++it)
        i++;
    return i;
}

void Server::sendPrivateMessage(int client_fd, const std::string &target, const std::string &msg)
{
    if (target[0] == '#')
    {
        if (_channels.find(target) == _channels.end())
        {
            sendToClient(client_fd, "403 " + target + " :No such channel\r\n");
            return;
        }
        if (!_channels[target].isMember(client_fd))
        {
            sendToClient(client_fd, "442 " + target + " :You're not on that channel\r\n");
            return;
        }
        std::string nick_sender = getNickname(client_fd);
        std::string msg_formatted = ":" + nick_sender + " PRIVMSG " + target + " : " + msg + "\r\n";

        for (std::set<int>::iterator it = _channels[target].getMembers().begin(); it != _channels[target].getMembers().end(); ++it)
        {
            int member_fd = *it;
            if (member_fd != client_fd)
                sendToClient(member_fd, msg_formatted);
        }
        return ;
    }
    else
    {
        int target_fd = getFdByNickname(target);
        if (target_fd < 0)
        {
            sendToClient(client_fd, "401 " + target + " :No such nick\r\n");
            return;
        }
        std::string nick_sender = getNickname(client_fd);
        std::string msg_formatted = ":" + nick_sender + " PRIVMSG " + target + " : " + msg + "\r\n";
        sendToClient(target_fd, msg_formatted);
        return;
    }
}

bool Server::isCorrectPasswordServer(const std::string &pass)
{
    if (pass == this->_serverPassword)
        return true;
    return false;
}

void Server::broadcastToChannel(const std::string &channel_name, const std::string &message, int to_ignore)
{
    if(_channels.find(channel_name) == _channels.end())
        return ;

    std::set<int> members = _channels[channel_name].getMembers();
    for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it)
		if (*it != to_ignore)
        	sendToClient(*it, message);
}

int Server::getFdByNickname(const std::string & target)
{
    for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getNickname() == target)
            return it->first;
    }
    return -1;
}

std::string Server::getNickname(int clientFd) const
{
    std::map<int, Client>::const_iterator it = _clients.find(clientFd);
    if (it == _clients.end())
        return "";
    return it->second.getNickname();
}

void Server::sendToClient(int client_fd, const std::string &response)
{
    _clients[client_fd].getBufferOut() += response;
    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        if (_poll_fds[i].fd == client_fd)
        {
            _poll_fds[i].events |= POLLOUT;
            break;
        }
    }
}

void Server::handleWriteEvent(int client_fd)
{
    Client &client = _clients[client_fd];
    //If the output buffer is empty, there is nothing to send.
    //We then deactivate the POLLOUT flag for this client.
    if (client.getBufferOut().empty())
    {
        disableWriteEvent(client_fd);
        return;
    }
    //We attempt to send the contents of the output buffer to the socket.
    ssize_t bytes_sent = send(client_fd, client.getBufferOut().c_str(), client.getBufferOut().size(), 0);
    //If the sending was successful and a few bytes were transmitted
    if (bytes_sent > 0)
    {
        client.getBufferOut().erase(0, bytes_sent);
        if (client.getBufferOut().empty())
            disableWriteEvent(client_fd);
    }
    else if (bytes_sent == -1)
    {
        //If the error is not EAGAIN or EWOULDBLOCK,This is a critical error and we delete the client.
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            std::cerr << "Error: send failed to FD " << client_fd << " (" << strerror(errno) << ")" << std::endl;
            removeClient(client_fd);
        }
        //Otherwise, EAGAIN or EWOULDBLOCK simply means that the socket is not ready to write at the moment, and we will wait for the next POLLOUT.
    }
}

void Server::disableWriteEvent(int clients_fd)
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        if (_poll_fds[i].fd == clients_fd)
        {
            _poll_fds[i].events &= ~POLLOUT;
            break;
        }
    }
}

void Server::removeClient(int client_fd)
{
    close(client_fd);
    _clients.erase(client_fd);

    for (std::vector<struct pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
    {
        if (it->fd == client_fd)
        {
            _poll_fds.erase(it);
            break;
        }
    }
    std::cout << "Client : " << client_fd << " Remove" << std::endl;
}

Server::~Server(void)
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
        close(_poll_fds[i].fd);
}

Server &Server::operator=(const Server &other)
{
    if (this != &other)
    {
        _server_fd = other._server_fd;
        _poll_fds = other._poll_fds;
        _clients = other._clients;
    }
    return(*this);
}

void Server::authenticate(int client_fd) {
	Client &client = _clients[client_fd];

	if (client.getRightPass() && !client.getUsername().empty() && !client.getNickname().empty())
	{
		client.setAuthenticated(true);
		sendToClient(client_fd, RPL_WELCOME(client.getNickname()));
	}
	else
	{
		if (!client.getRightPass())
			sendToClient(client_fd, "Need a password to be fully authenticated (/PASS <password>)\r\n");
		if (client.getUsername().empty())
			sendToClient(client_fd, "Need an username to be fully authenticated (/USER <username> 0 * <realname>)\r\n");
		if (client.getNickname().empty())
			sendToClient(client_fd, "Need a nickname to be fully authenticated (/NICK <nickname>)\r\n");
	}
}
