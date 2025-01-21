/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:49:59 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/17 10:49:59 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::topic(std::istringstream &iss, int client_fd) {
	//si le topic nest pas locked
	std::string name_channel;
	iss >> name_channel;
	if (name_channel.empty())
	{
		std::string response = "461 TOPIC :No recipient given\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	std::string topic;
	std::getline(iss, topic);
	if (!topic.empty() && (topic[0] == ' ' || topic[0] == ':'))
		topic.erase(0, 1);
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
	//check if is operator +t
	if (topic.empty())
	{
		std::string top = _channels[name_channel].getTopic();
		if (top.empty())
		{
			std::string response = "331 " + name_channel + "TOPIC :No topic is set\r\n";
			sendToClient(client_fd, response);
		}
		else
		{
			std::string response = "331 " + name_channel + " :" + top + "\r\n";
			sendToClient(client_fd, response);
		}
	}
	else
	{
		_channels[name_channel].setTopic(topic);
		std::string msg = ":" + getNickname(client_fd) + " TOPIC " + name_channel + " :" + topic + "\r\n";
		broadcastToChannel(name_channel, msg);
	}
	return true;
}
