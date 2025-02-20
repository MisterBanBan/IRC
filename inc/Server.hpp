/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 15:04:07 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 10:46:47 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include "replies.hpp"
#include <sstream> //istringstream
#include <cstdlib> //atoi
#include <cstring> //memset
#include <cerrno>
#include "ChessBot.hpp"




class Server
{
    public:
        Server(void);
        Server(const Server &other);
        int		initServerSocket(const std::string & port, const std::string & pass);
        void	acceptNewClient();
        void	clientData(int client_fd);
        void	sendToClient(int client_fd, const std::string &reponse);
        void	removeClient(int client_fd);
        int		getFdByNickname(const std::string & target);
        std::string getNickname(int clientFd) const;
		std::string getUsername(int clientFd) const;
		std::string getRealname(int clientFd) const;
        void	broadcastToChannel(const std::string &channel_name, const std::string &message, int to_ignore);
        void	sendPrivateMessage(int client_fd, const std::string &target, const std::string &msg);
        bool	isCorrectPasswordServer(const std::string &pass);
        int		getNbUser(int client_fd, const std::string &channel);
        void	handleWriteEvent(int client_fd);
        void	disableWriteEvent(int client_fd);
		void	authenticate(int client_fd);
		void	leaveChannel(const std::string & channel_name, int client_fd);

		void	nick(std::istringstream & iss, int client_fd);
		void	user(std::istringstream & iss, int client_fd);
		void	pass(std::istringstream & iss, int client_fd);
		void	join(std::istringstream & iss, int client_fd);
		void	kick(std::istringstream & iss, int client_fd);
		void	part(std::istringstream & iss, int client_fd);
		void	privmsg(std::istringstream & iss, int client_fd);
		void	topic(std::istringstream & iss, int client_fd);
		void	invite(std::istringstream & iss, int client_fd);
		void	mode(std::istringstream & iss, int client_fd);
        void    cap(std::istringstream &iss, int client_fd);
        void    who(std::istringstream &iss, int client_fd);
        void    quit(std::istringstream &iss, int client_fd);
        void    processBotMessage(int client_fd, std::string &msg);
        void    stop();
        std::vector<std::string> split(const std::string &s, char delimiter);
        void run();
        ~Server(void);
        Server &operator=(const Server &other);
    private:
        bool _running;
        int _server_fd;
        std::vector<struct pollfd> _poll_fds;
        std::map<std::string, Channel> _channels;
        std::map<int, Client> _clients;
        std::map<int, ChessBot> _ChessTable;
        std::string _serverHashPassword;
};



#endif