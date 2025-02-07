/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:13 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/05 17:48:21 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
// quand un client se co rajouter sil y a un topic le sujet du topic

bool Server::join(std::istringstream &iss, int client_fd)
{
	std::string channel_str;
	iss >> channel_str;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_ALREADYREGISTERED);
		return false;
	}
	if (channel_str.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS("JOIN"));
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
			if (_channels[channel_name].hasLimitUser() && _channels[channel_name].getUserLimit() == getNbUser(client_fd, channel_name))
			{
				sendToClient(client_fd, ERR_CHANNELISFULL(channel_name));
				return true;
			}

			if (_channels[channel_name].hasKey())
			{
                std::string provided_key = (i < keys.size()) ? keys[i] : "";
                if (!_channels[channel_name].isValidKey(provided_key)) {
					sendToClient(client_fd, ERR_BADCHANNELKEY(channel_name));
                    continue;
                }
            }
            if (!_channels[channel_name].isInviteOnly())
                _channels[channel_name].addMember(client_fd);
            else
			{
                if (!_channels[channel_name].isInvited(client_fd))
				{
                    sendToClient(client_fd, ERR_INVITEONLYCHAN(channel_name));
                    continue;
                }
                _channels[channel_name].addMember(client_fd);
            }
        }

		Channel &chan = _channels[channel_name];

		_clients[client_fd].getChannels().insert(channel_name);

		sendToClient(client_fd, JOIN(getNickname(client_fd), channel_name));
		if (!chan.getTopic().empty())
			sendToClient(client_fd, RPL_TOPIC(channel_name, chan.getTopic()));

		std::stringstream users;

		std::set<int> members = chan.getMembers();
		for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it)
		{
			if (it != members.begin())
				users << " ";
			users << getNickname(*it);
		}

		std::cout << chan.getName() << std::endl;
		sendToClient(client_fd, RPL_NAMREPLY(getNickname(client_fd), channel_name, users.str()));
		sendToClient(client_fd, RPL_ENDOFNAMES(channel_name));
	}
	return true;
}
