/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 15:14:33 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/05 09:57:56 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include <csignal>
#include "Server.hpp"

Server *globalServer = NULL;

void signal_handler(int signum, siginfo_t *info, void *context)
{
	(void)info;
	(void)context;

	if (signum == 2)
	{
		if (globalServer)
			globalServer->stop();
		std::cout << "\nServer is closed" << std::endl;
	}
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv <6667> <PASSWORD>" << std::endl;
        return 1;
    }
    std::string port = argv[1];
    std::string pass = argv[2];

    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
      std::cerr << "Error init handler SIGINT" << std::endl;
      return 1;
    }
    Server server;
    globalServer = &server;
    if (server.initServerSocket(port, pass))
		  return 1;
    server.run();
    return 0;
}