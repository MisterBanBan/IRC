/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:13 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/17 10:49:13 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::join(std::istringstream &iss, int client_fd) {
	std::string channel_name;
	iss >> channel_name;
	if (channel_name.empty())
	{
		std::string response = "JOIN :Not enough parameters\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	std::cout << "Size: " << _channels.size() << std::endl;
	if (_channels.find(channel_name) == _channels.end())
	{
		std::cout << "Test" << std::endl;
		Channel newChannel(channel_name);
		newChannel.addMember(client_fd);
		_channels[channel_name] = newChannel;
	}
	else
	{
		std::cout << "Test2" << std::endl;
		if (_channels[channel_name].inviteOnly == false)
		{
			if (_channels[channel_name].hasKey == false)
				_channels[channel_name].addMember(client_fd);
			else
			{
				std::string pass;
				iss >> pass;
				if (_channels[channel_name].key == pass)
				{
					if (_channels[channel_name].limitUser == false || _channels[channel_name].userLimit > getNbUser(client_fd, channel_name))
						_channels[channel_name].addMember(client_fd);
					else
					{
						std::string response = "JOIN :This channel has reached its limit\r\n";
						sendToClient(client_fd, response);
						return false;
					}
				}
				else
				{
					std::string response = "JOIN :This channel needs a key ex: JOIN #channel <key>\r\n";
					sendToClient(client_fd, response);
					return false;
				}
			}
		}
		else
		{
			//ici faire linvitation si luser est deja inviter
			std::string response = "JOIN :This channel " + channel_name + " is INVITE only\r\n";
			sendToClient(client_fd, response);
			return false;
		}
	}
	_clients[client_fd].channels.insert(channel_name);
	std::string response = ":" + _clients[client_fd].nickname
						   + " JOIN "
						   + channel_name
						   + "\r\n";
	sendToClient(client_fd, response);
	return true;
}
