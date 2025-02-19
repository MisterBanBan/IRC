/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:59 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/05 15:43:18 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::topic(std::istringstream &iss, int client_fd) {
	std::string name_channel;
	iss >> name_channel;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED(getNickname(client_fd)));
		return false;
	}

	if (name_channel.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "TOPIC"));
		return true;
	}
	if (_channels.find(name_channel) == _channels.end())
	{
		sendToClient(client_fd, ERR_NOSUCHCHANNEL(getNickname(client_fd), name_channel));
		return true;
	}
	if (!_channels[name_channel].isMember(client_fd))
	{
		sendToClient(client_fd, ERR_NOTONCHANNEL(getNickname(client_fd), name_channel));
		return true;
	}
	if (_channels[name_channel].isTopicLocked() && !_channels[name_channel].isOperator(client_fd))
	{
		sendToClient(client_fd, ERR_CHANOPRIVSNEEDED(getNickname(client_fd), name_channel));
		return true;
	}
	std::string topic;

	std::getline(iss, topic);
	if (topic.empty())
	{
		std::string actualTopic = _channels[name_channel].getTopic();
		if (actualTopic.empty())
			sendToClient(client_fd, RPL_NOTOPIC(getNickname(client_fd), name_channel));
		else
			sendToClient(client_fd, RPL_TOPIC(getNickname(client_fd), name_channel, actualTopic));
		return true;
	}

	while (!topic.empty() && (topic[0] == ' ' || topic[0] == ':'))
	{
		if (topic[0] == ':')
		{
			topic.erase(0, 1);
			if (topic.empty())
			{
				std::string actualTopic = _channels[name_channel].getTopic();
				if (actualTopic.empty())
					sendToClient(client_fd, RPL_NOTOPIC(getNickname(client_fd), name_channel));
				else
					sendToClient(client_fd, RPL_TOPIC(getNickname(client_fd), name_channel, actualTopic));
				return true;
			}
			else
			{
				_channels[name_channel].setTopic(topic);
				broadcastToChannel(name_channel, TOPIC(getNickname(client_fd), name_channel, topic), -1);
				return true;
			}
		}
		topic.erase(0, 1);
	}
	return true;
}
