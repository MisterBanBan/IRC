/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:59 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/20 13:30:53 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::topic(std::istringstream &iss, int client_fd) {
	std::string name_channel;
	iss >> name_channel;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED(getNickname(client_fd)));
		return;
	}

	if (name_channel.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS(getNickname(client_fd), "TOPIC"));
		return;
	}
	if (_channels.find(name_channel) == _channels.end())
	{
		sendToClient(client_fd, ERR_NOSUCHCHANNEL(getNickname(client_fd), name_channel));
		return;
	}

	Channel &chan = _channels[name_channel];
	if (!chan.isMember(client_fd))
	{
		sendToClient(client_fd, ERR_NOTONCHANNEL(getNickname(client_fd), name_channel));
		return;
	}
	if (chan.isTopicLocked() && !chan.isOperator(client_fd))
	{
		sendToClient(client_fd, ERR_CHANOPRIVSNEEDED(getNickname(client_fd), name_channel));
		return;
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
		return;
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
				return;
			}
			else
			{
				_channels[name_channel].setTopic(topic);
				broadcastToChannel(name_channel, TOPIC(getNickname(client_fd), name_channel, topic), -1);
				return;
			}
		}
		topic.erase(0, 1);
	}
	return;
}
