/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:13 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/01 18:47:44 by mtbanban         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
// quand un client se co rajouter sil y a un topic le sujet du topic

bool Server::join(std::istringstream &iss, int client_fd)
{
	std::string channel_str;
	iss >> channel_str;
	if (channel_str.empty())
	{
		std::string response = "JOIN: Not enough parameters\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	std::vector<std::string> channels = split(channel_str, ',');
	std::string keys_str;
	iss >> keys_str;
	std::vector<std::string> keys;
	if (!keys_str.empty())
		keys = split(keys_str, ',');
		
	for (size_t i = 0; i < channels.size(); i++)
	{
		std::string channel_name = channels[i];
		if (channel_name[0] != '#')
		{
			std::string response = "JOIN: Channel format -> <#channel>\r\n";
			sendToClient(client_fd, response);
			continue;
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
					std::string provided_key = (i < keys.size()) ? keys[i] : "";
                    if (_channels[channel_name].getKey() == provided_key)
                        _channels[channel_name].addMember(client_fd);
					else
					{
						std::string response;
						if (provided_key.empty())
							response = "JOIN: This channel needs a pass (JOIN #channel <pass>)\r\n";
						else
							response = "JOIN: Invalid password\r\n";
						sendToClient(client_fd, response);
						return false;
					}
				}
			}
			else
			{
				if (!_channels[channel_name].isInvited(client_fd))
				{
					std::string response = "JOIN: This channel " + channel_name + " is on INVITE only\r\n";
					sendToClient(client_fd, response);
					continue;
				}
				_channels[channel_name].addMember(client_fd);
			}
		}
		_clients[client_fd].channels.insert(channel_name);
		std::string response = ":" + _clients[client_fd].nickname
							+ " JOIN "
							+ channel_name
							+ "\r\n";
		sendToClient(client_fd, response);
	}
	return true;
}
