/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:13 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 11:18:07 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::join(std::istringstream &iss, int client_fd)
{
	std::string channel_str;
	iss >> channel_str;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_ALREADYREGISTERED(getNickname(client_fd)));
		return;
	}
	if (channel_str.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "JOIN"));
		return;
	}
	std::vector<std::string> channels = split(channel_str, ',');

	if (channels.size() == 1 && channels[0] == "0")
	{
		for (std::set<std::string>::iterator it = _clients[client_fd].getChannels().begin(); it != _clients[client_fd].getChannels().end(); it++) {
			broadcastToChannel(*it, PART(_clients[client_fd].getNickname(), *it, ":Leaving"), -1);
			leaveChannel(channel_str, client_fd);
		}
		return;
	}

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
			if (_channels[channel_name].hasUserLimit() && _channels[channel_name].getUserLimit() == _channels[channel_name].getNbMembers())
			{
				sendToClient(client_fd, ERR_CHANNELISFULL(getNickname(client_fd), channel_name));
				return;
			}

			if (_channels[channel_name].hasKey())
			{
                std::string provided_key = (i < keys.size()) ? keys[i] : "";
                if (!_channels[channel_name].isValidKey(provided_key)) {
					sendToClient(client_fd, ERR_BADCHANNELKEY(getNickname(client_fd), channel_name));
                    continue;
                }
            }
            if (!_channels[channel_name].isInviteOnly())
                _channels[channel_name].addMember(client_fd);
            else
			{
                if (!_channels[channel_name].isInvited(client_fd))
				{
                    sendToClient(client_fd, ERR_INVITEONLYCHAN(getNickname(client_fd), channel_name));
                    continue;
                }
                _channels[channel_name].addMember(client_fd);
            }
        }

		Channel &chan = _channels[channel_name];

		_clients[client_fd].getChannels().insert(channel_name);

		sendToClient(client_fd, JOIN(getNickname(client_fd), channel_name));
		if (!chan.getTopic().empty())
			sendToClient(client_fd, RPL_TOPIC(getNickname(client_fd), channel_name, chan.getTopic()));

		std::stringstream users;

		std::set<int> members = chan.getMembers();
		for (std::set<int>::iterator it = members.begin(); it != members.end(); ++it)
		{
			if (it != members.begin())
				users << " ";
			if (chan.isOperator(*it))
				users << "@";
			users << getNickname(*it);
		}

		sendToClient(client_fd, RPL_NAMREPLY(getNickname(client_fd), channel_name, users.str()));
		sendToClient(client_fd, RPL_ENDOFNAMES(getNickname(client_fd), channel_name));
	}
	return;
}
