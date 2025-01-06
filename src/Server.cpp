#include "Server.hpp"
#include "Client.hpp"
#include <arpa/inet.h>
#include <sstream>

Server::Server(void) { }

Server::Server(const Server &other)
{
    *this = other;
}

void Server::initServerSocket(const std::string port, const std::string pass)
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
    if (pass.empty())
    {
        std::cout << "Error: Password is empty" << std::endl;
        return ;
    }
    else
        this->_serverPassword = pass;
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
    // _poll_fds is used to store all file descriptors (sockets) that the server wants to monitor for events.
    struct pollfd server_pollfd;
    server_pollfd.fd = _server_fd; //Assigns the server socket file descriptor (_server_fd) to the fd field of the pollfd structure.
    server_pollfd.events = POLLIN; // Sets the POLLIN event type: Indicates that we are monitoring read events.
    server_pollfd.revents = 0; //This field is used by poll() to indicate which events actually occurred.
    _poll_fds.push_back(server_pollfd); //This allows the server to detect when a new connection is ready to be accepted
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
        //_poll_fds.data() to the first element of the _poll_fds vector, which contains the pollfd structures describing the file descriptors to monitor
        int ret = poll(_poll_fds.data(), _poll_fds.size(), -1); // Timeout infini if ret < 0 error
        if(ret < 0)
        {
            std::cout << "Error: poll failde" << std::endl;
            break;
        }
        for (size_t i = 0; i < _poll_fds.size(); ++i)
        {
            if (_poll_fds[i].revents & POLLIN)
            {
                if (_poll_fds[i].fd == _server_fd)
                {
                    acceptNewClient();
                } 
                else 
                {
                    clientData(_poll_fds[i].fd);
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

    _clients[client_fd].buffer_in.append(buffer, receive);
    size_t pos;
/*en attendant dtre sur un vrai hexchat  while ((pos = _clients[client_fd].buffer_in.find("\r\n")) != std::string::npos)
    {
        std::string command = _clients[client_fd].buffer_in.substr(0, pos);
        _clients[client_fd].buffer_in.erase(0, pos + 2);*/
    while ((pos = _clients[client_fd].buffer_in.find("\n")) != std::string::npos)
    {
        std::string command = _clients[client_fd].buffer_in.substr(0, pos);
        _clients[client_fd].buffer_in.erase(0, pos + 1);

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
                std::string response = "431 :No nickname given\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            if (_clients[client_fd].is_authenticated || !_clients[client_fd].nickname.empty())
            {
                std::string response = "462 :You are already registered\r\n";
                sendToClient(client_fd, response);
                return;
            }
            int target_fd = getFdByNickname(nickname);
            if (target_fd > 0)
            {
                sendToClient(client_fd, "NICK :Your nickname already exist\r\n");
                return;
            }
            _clients[client_fd].nickname = nickname;
            if (!_clients[client_fd].user.empty())
            {
                _clients[client_fd].is_authenticated = true;
                sendToClient(client_fd, ":server 001 " + _clients[client_fd].nickname + " :Welcome!\r\n");
            }
            else
                sendToClient(client_fd, ":server NOTICE * :Please set your USER\r\n");
            std::string response = "Nickname set to " + nickname + "\r\n";
            sendToClient(client_fd, response);
        }
        else if (cmd == "USER")
        {
            std::string user;
            std::string realname;
            iss >> user;
            if (user.empty())
            {
                std::string response = "Error: No user given\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            std::getline(iss, realname);
            if (!realname.empty())
            {
                while (!realname.empty() && (realname[0] == ' ' || realname[0] == ':'))
                    realname.erase(0, 1);
            }
            else   
                realname = "Unknow";
            if (_clients[client_fd].is_authenticated || !_clients[client_fd].user.empty())
            {
                std::string response = "462 :You are already registered\r\n";
                sendToClient(client_fd, response);
                return;
            }
            _clients[client_fd].user = user;
            _clients[client_fd].realname = realname;
            if (!_clients[client_fd].nickname.empty())
            {
                _clients[client_fd].is_authenticated = true;
                sendToClient(client_fd, ":server 001 " + _clients[client_fd].nickname + " :Welcome!\r\n");
            }
            else
                sendToClient(client_fd, ":server NOTICE * :Please set your NICK\r\n");
            std::stringstream ss;
            ss << "USER command from FD " << client_fd 
            << " => username: " << _clients[client_fd].user
            << ", realname: " << _clients[client_fd].realname;
            std::string response = ss.str() + "\r\n";
            sendToClient(client_fd, response);
        }
        else if (cmd == "PASS")
        {
            std::string pass;
            iss >> pass;
            if (pass.empty())
            {
                std::string response = "461 PASS :Not enough parameters\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            if (_clients[client_fd].is_authenticated)
            {
                sendToClient(client_fd, "462 PASS:You are already registered\r\n");
                continue;
            }
            if (!isCorrectPasswordServer(pass))
            {
                std::string response = "464 PASS:Password incorrect\r\n";
                sendToClient(client_fd, response);
                //removeClient(client_fd);
                continue;
            }
            std::string response = "NOTICE * :Password accepted\r\n";
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
            if (_channels.find(channel_name) == _channels.end())
            {
                Channel newChannel(channel_name);
                newChannel.addMember(client_fd);
                _channels[channel_name] = newChannel;
            }
            else
                _channels[channel_name].addMember(client_fd);

            _clients[client_fd].channels.insert(channel_name);
            std::string response = ":" + _clients[client_fd].nickname
                         + " JOIN "
                         + channel_name
                         + "\r\n";
            sendToClient(client_fd, response);
        }
        else if (cmd == "KICK")
        {
            std::string channel_name;
            std::string target_nick;
            std::string reason;
            iss >> channel_name >> target_nick >> reason;
            if(channel_name.empty() || target_nick.empty())
            {
                std::string response = "KICK :Not enough parameters";
                sendToClient(client_fd, response);
                return;
            }
            std::getline(iss, reason); 
            if (!reason.empty())
            {
                if (reason[0] == ' ')
                    reason.erase(0,1);
            }
            if (reason.empty())
                reason = "No reason";
            
            if (_channels.find(channel_name) == _channels.end())
            {
                std::string response = "KICK :This channel doesn't exist";
                sendToClient(client_fd, response);
                return;
            }
            int target_fd = getFdByNickname(target_nick);
            if (target_fd < 0)
            {
                sendToClient(client_fd, "KICK :No such nickname\r\n");
                return;
            }
            if (!_channels[channel_name].isMember(target_fd))
            {
                sendToClient(client_fd, "KICK :Target not in channel\r\n");
                return;
            }
            if (!_channels[channel_name].isMember(client_fd))
            {
                sendToClient(client_fd, "KICK :You are not in this channel\r\n");
                return;
            }
            _channels[channel_name].removeMember(target_fd);
            _clients[target_fd].channels.erase(channel_name);
            std::string kicker_nick = _clients[client_fd].nickname;
            std::stringstream msg;
            msg << ":" << kicker_nick
                << " KICK " << channel_name
                << " "     << target_nick
                << " :"    << reason
                << "\r\n";
            broadcastToChannel(channel_name, msg.str());
            sendToClient(target_fd, msg.str());
        }
        else if(cmd == "PART")
        {
            std::string channel_name;
            std::string reason;
            if(channel_name.empty())
            {
                std::string response = "461 PART :Not enough parameters\r\n";
                sendToClient(client_fd, response);
                return;
            }
            std::getline(iss, reason); 
            if (!reason.empty())
            {
                if (reason[0] == ' ')
                    reason.erase(0,1);
            }
            if (reason.empty())
                reason = "No reason";
            if (_channels.find(channel_name) == _channels.end())
            {
                std::string response = "403 " + channel_name + " :No such channel\r\n";
                sendToClient(client_fd, response);
                return;
            }
            if (!_channels[channel_name].isMember(client_fd))
            {
                sendToClient(client_fd, "442 " + channel_name + " :You're not on that channel\r\n");
                return;
            }
            _channels[channel_name].removeMember(client_fd);
            _clients[client_fd].channels.erase(channel_name);
            std::string part_nick = _clients[client_fd].nickname;
            std::stringstream msg;
            msg << ":" << part_nick
                << " PART " << channel_name
                << " "     << part_nick
                << " :"    << reason
                << "\r\n";
            broadcastToChannel(channel_name, msg.str());
            sendToClient(client_fd, msg.str());
        }
        else if (cmd == "PRIVMSG")
        {
            std::string target;
            iss >> target;
            if (target.empty())
            {
                std::string response = "411 PRIVMSG :No recipient given\r\n";
                sendToClient(client_fd, response);
                return;
            }
            std::string msg;
            std::getline(iss, msg);
            while (!msg.empty() && (msg[0] == ' ' || msg[0] == ':'))
                msg.erase(0, 1);
            if (msg.empty())
            {
                std::string response = "412 PRIVMSG :No text to send\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            sendPrivateMessage(client_fd, target, msg);
        }
        else if (cmd == "TOPIC")
        {
            std::string name_channel;
            iss >> name_channel;
            if (name_channel.empty())
            {
                std::string response = "461 TOPIC :No recipient given\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            std::string topic;
            std::getline(iss, topic);
            if (!topic.empty() && (topic[0] == ' ' || topic[0] == ':'))
                topic.erase(0, 1);
            if (_channels.find(name_channel) == _channels.end())
            {
                std::string response = "403 " + name_channel + "TOPIC :No such channel\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            if (!_channels[name_channel].isMember(client_fd))
            {
                std::string response = "442 " + name_channel + "TOPIC :You're not on that channel\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            //check if is operator +t
            if (topic.empty())
            {
                std::string top = _channels[name_channel].getTopic();
                if (top.empty())
                {
                    std::string response = "331 " + name_channel + "TOPIC :No topic is set\r\n";
                    sendToClient(client_fd, response);
                }
                else
                {
                    std::string response = "331 " + name_channel + " :" + top + "\r\n";
                    sendToClient(client_fd, response);
                }
            }
            else
            {
                _channels[name_channel].setTopic(topic);
                std::string msg = ":" + getNickname(client_fd) + " TOPIC " + name_channel + " :" + topic + "\r\n";
                broadcastToChannel(name_channel, msg);
            }

        }
        else if (cmd == "INVITE")
        {
            std::string nickname, channelName;
            iss >> nickname >> channelName;
            if (nickname.empty() || channelName.empty())
            {
                std::string response = "461 INVITE :Not enough parameters\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            if (_channels.find(channelName) == _channels.end())
            {
                std::string response = "403 " + channelName + "INVITE :No such channel\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            if(!_channels[channelName].isMember(client_fd))
            {
                std::string response = "442 " + channelName + "INVITE :You're not on that channel\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            int target = getFdByNickname(nickname);
            if (target < 0)
            {
                std::string response = "401 " + channelName + "INVITE :No such nick\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            std::string response = "341 " + getNickname(client_fd) + " " + nickname + " " + channelName + "\r\n";
            sendToClient(client_fd, response);
            std::string notice = ":" + getNickname(client_fd) + " INVITE " + nickname + " :" + channelName + "\r\n";
            sendToClient(target, notice);
        }
        else if (cmd == "MODE")
        {
            std::string channelName, modes;
            iss >> channelName >> modes;
            if (channelName.empty())
            {
                std::string response = "461 MODE :Not enough parameters\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            if (_channels.find(channelName) == _channels.end())
            {
                std::string response = "403 " + channelName + "MODE :No such channel\r\n";
                sendToClient(client_fd, response);
                continue;
            }
            if (modes.empty())
            {
                std::string mode = "-i Set/Remove Channel to Invite Only \r\n -t : Set/remove TOPIC command restrictions for channel operators \r\n -k: Set/delete channel key (password) \r\n -o: Grant/withdraw channel operator privilege \r\n -l : Set/remove user limit for channel";
                std::string response = mode + "324 " + channelName + "MODE + (some modes)\r\n";
                sendToClient(client_fd, response);
                continue;
            }

            //il faut rajouter les different mode
        }
        else
        {
            std::string response = "421 " + cmd + " :Unknown command\r\n";
            sendToClient(client_fd, response);
        }
    }
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

void Server::broadcastToChannel(const std::string &channel_name, const std::string &message)
{
    if(_channels.find(channel_name) == _channels.end())
        return ;

    std::set<int> members = _channels[channel_name].getMembers();
    for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it)
        sendToClient(*it, message);
}

int Server::getFdByNickname(const std::string target)
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
