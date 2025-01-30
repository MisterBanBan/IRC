/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:13 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/29 12:03:35 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::join(std::istringstream &iss, int client_fd) {
	std::string channel_name;
	iss >> channel_name;
	if (channel_name.empty())
	{
		std::string response = "JOIN: Not enough parameters\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	if (channel_name[0] != '#')
	{
		std::string response = "JOIN: Channel format -> <#channel>\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	if (_channels.find(channel_name) == _channels.end())
	{
		Channel newChannel(channel_name);
		newChannel.addMember(client_fd);
		_channels[channel_name] = newChannel;
		_channels[channel_name].getOperators().insert(client_fd);
	}
	else
	{
		if (_channels[channel_name].getLimitUser() && _channels[channel_name].getUserLimit() == getNbUser(client_fd, channel_name))
		{
			std::string response = "JOIN: This channel has reached its limit\r\n";
			sendToClient(client_fd, response);
			return false;
		}

		if (!_channels[channel_name].getInviteOnly())
		{
			if (!_channels[channel_name].getHasKey())
				_channels[channel_name].addMember(client_fd);
			else
			{
				std::string pass;
				iss >> pass;
				if (_channels[channel_name].getKey() == pass)
					_channels[channel_name].addMember(client_fd);
				else
				{
					std::string response;
					if (pass.empty())
						response = "JOIN: This channel needs a pass (JOIN #channel <pass>)\r\n";
					else
						response = "JOIN: Invalid password\r\n";
					sendToClient(client_fd, response);
					return false;
				}
			}
		}
		else
			if (!_channels[channel_name].isInvited(client_fd))
			{
				std::string response = "JOIN: This channel " + channel_name + " is on INVITE only\r\n";
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
