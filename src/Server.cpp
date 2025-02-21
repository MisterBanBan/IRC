/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 12:43:53 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/21 09:16:21 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include <arpa/inet.h>
#include <crypt.h>

Server::Server(void) : _running(true) { }

Server::Server(const Server &other)
{
    *this = other;
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

void Server::stop()
{
    _running = false;
}

int	Server::initServerSocket(const std::string & port, const std::string & pass)
{
	char *end;

	if (port.size() != 4)
	{
		std::cout << "Error: Port needs to contain 4 numbers" << std::endl;
		return 1;
	}

    int portInt = strtol(port.c_str(), &end, 10);
    if (*end != '\0')
    {
        std::cout << "Error: Port is invalid" << std::endl;
        return 1;
    }

    if (pass.empty())
    {
        std::cout << "Error: Password is empty" << std::endl;
        return 1;
    }

	this->_serverHashPassword = crypt(pass.c_str(), "$6$RNEuivJ08k");
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
    {
        std::cout << "Error: Socket failed (" << errno << ")" << std::endl;
        return 1;
    }
    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR ,&opt, sizeof(opt)) == -1)
	{
		std::cout << "Error: " << errno << std::endl;
		return 1;
	}
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portInt);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
   if (bind(_server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Error: bind failed (" << strerror(errno) << ")" << std::endl;
        return 1;
    }
    if (listen(_server_fd, SOMAXCONN))
    {
        std::cout << "Error: listen failed" << std::endl;
        return 1;
    }
    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cout << "Error: F_SETFL failed" << std::endl;
        return 1;
    }
    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    _poll_fds.push_back(server_pollfd);
	return 0;
}

void Server::run()
{
	std::cout << "Server is running" << std::endl;

	while (_running)
	{
		int ret = poll(_poll_fds.data(), _poll_fds.size(), -1);
		if (ret < 0 && _running)
		{
			std::cout << "Error: poll failed" << std::endl;
			break;
		}
		for (size_t i = 0; i < _poll_fds.size();)
		{
			int fd = _poll_fds[i].fd;
			short revents = _poll_fds[i].revents;
			if (revents & (POLLERR | POLLHUP | POLLNVAL))
			{
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
			if (revents & POLLOUT)
				handleWriteEvent(fd);
			++i;
		}
	}
	close(_server_fd);
}

void Server::acceptNewClient()
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) 
    {
        if (errno != EWOULDBLOCK && errno != EAGAIN)
        {
            std::cout << "Error: accept failed (" << strerror(errno) << ")" << std::endl;
        }
        return;
    }
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
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
            nick(iss, client_fd);
        else if (cmd == "USER")
            user(iss, client_fd);
        else if (cmd == "PASS")
            pass(iss, client_fd);
        else if (cmd == "JOIN")
            join(iss, client_fd);
        else if (cmd == "KICK")
            kick(iss, client_fd);
        else if(cmd == "PART")
            part(iss, client_fd);
        else if (cmd == "PRIVMSG")
            privmsg(iss, client_fd);
        else if (cmd == "TOPIC")
            topic(iss, client_fd);
        else if (cmd == "INVITE")
            invite(iss, client_fd);
        else if (cmd == "MODE")
            mode(iss, client_fd);
        else if (cmd == "CAP")
            cap(iss, client_fd);
        else if (cmd == "WHO")
            who(iss, client_fd);
        else if (cmd == "QUIT")
            quit(iss, client_fd);
        else
            sendToClient(client_fd, ERR_UNKNOWNCOMMAND(getNickname(client_fd), cmd));
    }
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

bool Server::isCorrectPasswordServer(const std::string &pass)
{
    if (crypt(pass.c_str(), "$6$RNEuivJ08k") == this->_serverHashPassword)
        return true;
    return false;
}

void Server::handleWriteEvent(int client_fd)
{
	Client &client = _clients[client_fd];
	if (client.getBufferOut().empty())
	{
		disableWriteEvent(client_fd);
		return;
	}
	ssize_t bytes_sent = send(client_fd, client.getBufferOut().c_str(), client.getBufferOut().size(), 0);
	if (bytes_sent > 0)
	{
		client.getBufferOut().erase(0, bytes_sent);
		if (client.getBufferOut().empty())
			disableWriteEvent(client_fd);
	}
	else if (bytes_sent == -1)
	{
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			std::cerr << "Error: send failed to FD " << client_fd << " (" << strerror(errno) << ")" << std::endl;
			removeClient(client_fd);
		}
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
			sendToClient(client_fd, "Need a password to be fully authenticated (PASS <password>)\r\n");
		if (client.getUsername().empty())
			sendToClient(client_fd, "Need an username to be fully authenticated (USER <username> 0 * :<realname>)\r\n");
		if (client.getNickname().empty())
			sendToClient(client_fd, "Need a nickname to be fully authenticated (NICK <nickname>)\r\n");
	}
}

void Server::leaveChannel(const std::string &channel_name, int client_fd) {
	_channels[channel_name].removeMember(client_fd);
	_clients[client_fd].getChannels().erase(channel_name);
	if (_channels[channel_name].getMembers().empty())
		_channels.erase(channel_name);
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

std::string Server::getUsername(int clientFd) const {
	std::map<int, Client>::const_iterator it = _clients.find(clientFd);
	if (it == _clients.end())
		return "";
	return it->second.getUsername();
}

std::string Server::getRealname(int clientFd) const {
	std::map<int, Client>::const_iterator it = _clients.find(clientFd);
	if (it == _clients.end())
		return "";
	return it->second.getRealname();
}

std::vector <std::string> Server::split(const std::string &s, char delimiter)
{
	std::vector<std::string> tokens;
	std::istringstream iss(s);
	std::string token;
	while (std::getline(iss, token, delimiter))
	{
		if (!token.empty())
			tokens.push_back(token);
	}
	return tokens;
}