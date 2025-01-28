/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 15:14:33 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/28 15:14:33 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv <6667> <PASSWORD>\n" << std::endl;
        return 1;
    }
    std::string port = argv[1];
    std::string pass = argv[2];

    Server server;
    if (server.initServerSocket(port, pass))
		return 1;
    server.run();
}