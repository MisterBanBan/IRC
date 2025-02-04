/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtbanban <mtbanban@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:59 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 14:30:37 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::topic(std::istringstream &iss, int client_fd) {
	//si le topic nest pas locked
	std::string name_channel;
	iss >> name_channel;

	if (!_clients[client_fd].isAuthenticated())
	{
		std::string response = "JOIN: You need to be authenticated to do that\r\n";
		sendToClient(client_fd, response);
		return false;
	}

	if (name_channel.empty())
	{
		std::string response = "461 TOPIC :Not enough parameters\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (_channels.find(name_channel) == _channels.end())
	{
		std::string response = "403 " + name_channel + "TOPIC :No such channel\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (!_channels[name_channel].isMember(client_fd))
	{
		std::string response = "442 " + name_channel + "TOPIC :You're not on that channel\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	if (_channels[name_channel].getTopicLocked() && !_channels[name_channel].isOperator(client_fd))
	{
		std::string response = "482 " + name_channel + "TOPIC :You're not channel operator\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	std::string topic;
	std::getline(iss, topic);
	while (!topic.empty() && (topic[0] == ' ' || topic[0] == ':'))
	{
		if (topic[0] == ':')
		{
			topic.erase(0, 1);
			if (topic.empty())
			{
				std::string top = _channels[name_channel].getTopic();
				if (top.empty())
				{
					std::string response = "331 " + name_channel + "TOPIC :No topic is set\r\n";
					sendToClient(client_fd, response);
					return true;
				}
				else
				{
					std::string response = "331 " + name_channel + " :" + top + "\r\n";
					sendToClient(client_fd, response);
					return true;
				}
			}
			else
			{
				_channels[name_channel].setTopic(topic);
				std::string msg = ":" + getNickname(client_fd) + " TOPIC " + name_channel + " :" + topic + "\r\n";
				broadcastToChannel(name_channel, msg, -1);
				return true;
			}
		}
		topic.erase(0, 1);
	}
	std::string response = "Error :The order is TOPIC #channel :topic\r\n";
	sendToClient(client_fd, response);
	return true;
}
