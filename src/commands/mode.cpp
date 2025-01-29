/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:50:18 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/29 11:16:52 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::mode(std::istringstream &iss, int client_fd) {
	std::string channelOrUser, modes;
	iss >> channelOrUser >> modes;
	if (channelOrUser.empty())
	{
		std::string response = "461 MODE :Not enough parameters\r\n";
		sendToClient(client_fd, response);
		return true;
	}

	if (channelOrUser[0] == '#')
	{
		if (_channels.find(channelOrUser) == _channels.end())
		{
			std::string response = "403 " + channelOrUser + "MODE :No such channel\r\n";
			sendToClient(client_fd, response);
			return true;
		}
		Channel &chan = _channels[channelOrUser];
		if (!chan.isMember(client_fd))
		{
			std::string response = "442 " + channelOrUser + "MODE :You're not on that channel\r\n";
			sendToClient(client_fd, response);
			return false;
		}
		if (!isOperator(client_fd, channelOrUser))
		{
			std::string response = "482 " + channelOrUser + "MODE :You're not channel operator\r\n";
			sendToClient(client_fd, response);
			return false;
		}
		bool add = false;
		for (size_t i = 0; i < modes.size(); i++)
		{
			std::cout << "Index: " << i << " | " << modes[i] << std::endl;
			if (modes[i] == '-')
				add = false;
			if (modes[i] == '+')
				add = true;
			if (modes[i] == '+' || modes[i] == '-')
				continue;
			switch(modes[i])
			{
				case 'i':
					chan.inviteOnly = add;
					continue;
				case 't':
					chan.topicLocked = add;
					continue;
				case 'k':
				{
					if (add)
					{
						if (chan.key.empty())
						{
							std::string key;
							iss >> key;
							if (!key.empty())
							{
								chan.hasKey = add;
								chan.key = key;
								std::string response = "MODE :Pass added for " + chan.getName() + "\r\n";
								sendToClient(client_fd, response);
							}
							else
							{
								std::string response = "MODE :Pass is missing\r\n";
								sendToClient(client_fd, response);
								return false;
							}
						}
						else
						{
							std::string response = "MODE :The channel " + chan.getName() + " already has an existing pass\r\n";
							sendToClient(client_fd, response);
							return false;
						}
					}
					else
					{
						chan.hasKey = false;
						chan.key.clear();
						std::string response = "MODE :The pass for " + chan.getName() + "has been removed\r\n";
						sendToClient(client_fd, response);
						return false;
					}
				}
				case 'l':
				{
					if (add)
					{
						std::string nbUser;
						iss >> nbUser;
						if (!nbUser.empty())
						{
							char *end = NULL;
							int nb = strtol(nbUser.c_str(), &end, 10);
							if (nb <= 0 || *end != '\0')
							{
								std::string response = "MODE :enter a number or a number greater than 0\r\n";
								sendToClient(client_fd, response);
								return false;
							}
							if (nb < getNbUser(client_fd, channelOrUser))
							{
								std::string response = "MODE :there are more users in the channel" + chan.getName() + "than the limit you are try to set. Please consider deleting users before setting the limit\r\n";
								sendToClient(client_fd, response);
								return false;
							}
							else
							{
								chan.limitUser = add;
								chan.userLimit = nb;
								std::string response = "MODE :Limit of users set for the channel " + chan.getName() + "\r\n";
								sendToClient(client_fd, response);
							}
						}
					}
					else
					{
						chan.limitUser = add;
						chan.userLimit = 0;
						std::string response = "MODE :removed channel's users limit\r\n";
						sendToClient(client_fd, response);
					}
					continue;
				}
				case 'o':
				{
					std::string opUser;
					iss >> opUser;
					int fd = getFdByNickname(opUser);
					if (fd < 0)
					{
						std::string response = "401 " + channelOrUser + " :No such nick\r\n";
						sendToClient(client_fd, response);
						return true;
					}
					if (!chan.isMember(fd))
					{
						std::string response = "441 " + channelOrUser + "They aren't on that channel\r\n";
						sendToClient(client_fd, response);
						return true;
					}
					if (add)
						chan.operators.insert(fd);
					else
						chan.operators.erase(fd);

				}
				default:
				{
					std::string response = "501 :Unknown MODE flag\r\n";
					sendToClient(client_fd, response);
					break;
				}
			}
		}
	}
	else
	{
		int target = getFdByNickname(channelOrUser);
		if (target < 0)
		{
			std::string response = "401 " + channelOrUser + "MODE :No such nick\r\n";
			sendToClient(client_fd, response);
			return true;
		}
		if (_clients.find(target) == _clients.end())
		{
			std::string response = "403 " + channelOrUser + "MODE :No such channel\r\n";
			sendToClient(client_fd, response);
			return true;
		}
	}

	if (modes.empty())
	{
		std::string mode = "-i Set/Remove Channel to Invite Only \r\n -t : Set/remove TOPIC command restrictions for channel operators \r\n -k: Set/delete channel key (password) \r\n -o: Grant/withdraw channel operator privilege \r\n -l : Set/remove user limit for channel";
		std::string response = mode + "324 " + channelOrUser + "MODE + (some modes)\r\n";
		sendToClient(client_fd, response);
		return true;
	}
	return true;
}
