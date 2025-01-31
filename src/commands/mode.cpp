/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:50:18 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/31 10:03:53 by mbaron-t         ###   ########.fr       */
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
			std::string response = "403 " + channelOrUser + " MODE :No such channel\r\n";
			sendToClient(client_fd, response);
			return true;
		}
		Channel &chan = _channels[channelOrUser];
		if (!chan.isMember(client_fd))
		{
			std::string response = "442 " + channelOrUser + " MODE :You're not on that channel\r\n";
			sendToClient(client_fd, response);
			return false;
		}
		if (!chan.isOperator(client_fd))
		{
			std::string response = "482 " + channelOrUser + " MODE :You're not channel operator\r\n";
			sendToClient(client_fd, response);
			return false;
		}
		bool add = false;

		while (!modes.empty())
		{
			for (size_t i = 0; i < modes.size(); i++)
			{
				if (i == 0)
				{
					if (modes[i] == '-')
						add = false;
					if (modes[i] == '+')
						add = true;
					if (modes[i] == '+' || modes[i] == '-')
						continue;
					else
					{
						std::string response = "MODE :Flags must start with '-' or '+'\r\n";
						sendToClient(client_fd, response);
						break;
					}
				}
				switch(modes[i])
				{
					case 'i':
						chan.setInviteOnly(add);
						continue;
					case 't':
						chan.setTopicLocked(add);
						continue;
					case 'k':
					{
						if (add)
						{
							std::string key;
							if (!iss.eof())
								iss >> key;
							if (!key.empty())
							{
								if (chan.getKey().empty())
								{
									chan.setHasKey(true);
									chan.setKey(key);
									std::string response = "MODE :Pass added for " + chan.getName() + "\r\n";
									sendToClient(client_fd, response);
									continue;
								}
								else
								{
									chan.setKey(key);
									std::string response = "MODE :Updated " + chan.getName() + "'s password\r\n";
									sendToClient(client_fd, response);
									continue;
								}
							}
							else
							{
								std::string response = "MODE :Not enough argument to set the channel's password\r\n";
								sendToClient(client_fd, response);
								continue;
							}
						}
						else
						{
							chan.setHasKey(false);
							chan.setKey("");
							std::string response = "MODE :The pass for " + chan.getName() + " has been removed\r\n";
							sendToClient(client_fd, response);
							continue;
						}
					}
					case 'l':
					{
						if (add)
						{
							std::string nbUser;
							if (!iss.eof())
								iss >> nbUser;
							if (!nbUser.empty())
							{
								char *end = NULL;
								int nb = strtol(nbUser.c_str(), &end, 10);
								if (nb <= 0 || *end != '\0')
								{
									std::string response = "MODE :Enter a number or a number greater than 0\r\n";
									sendToClient(client_fd, response);
									continue;
								}
								if (nb < getNbUser(client_fd, channelOrUser))
								{
									std::string response = "MODE :There are more users in the channel " + chan.getName() + " than the limit you are try to set. Please consider deleting users before setting the limit\r\n";
									sendToClient(client_fd, response);
									continue;
								}
								else
								{
									chan.setLimitUser(add);
									chan.setUserLimit(nb);
									std::string response = "MODE :Limit of users set for the channel " + chan.getName() + "\r\n";
									sendToClient(client_fd, response);
									continue;
								}
							}
							else
							{
								std::string response = "MODE :Not enough argument to set the channel's limit\r\n";
								sendToClient(client_fd, response);
								continue;
							}
						}
						else
						{
							chan.setLimitUser(add);
							chan.setUserLimit(0);
							std::string response = "MODE :Removed channel's users limit\r\n";
							sendToClient(client_fd, response);
							continue;
						}
					}
					case 'o':
					{
						std::string opUser;
						if (!iss.eof())
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
							std::string response = "441 " + channelOrUser + " :They aren't on that channel\r\n";
							sendToClient(client_fd, response);
							return true;
						}
						if (add)
						{
							chan.getOperators().insert(fd);
							std::string response = "MODE :Added " + opUser + " as an channel operator for " + chan.getName() + "\r\n";
							sendToClient(client_fd, response);
						}
						else
							chan.getOperators().erase(fd);
						continue;
					}
					default:
					{
						std::string response = "501 :Unknown MODE flag\r\n";
						sendToClient(client_fd, response);
						break;
					}
				}
			}
			if(iss.eof())
				break ;
			iss >> modes;
		}
	}
	else
	{
		// TODO
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
