/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:50:18 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 15:23:47 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::mode(std::istringstream &iss, int client_fd) {
	std::string channelOrUser, modes;
	iss >> channelOrUser >> modes;

	if (!_clients[client_fd].is_authenticated)
	{
		std::string response = "JOIN: You need to be authenticated to do that\r\n";
		sendToClient(client_fd, response);
		return false;
	}

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

		if (modes.empty())
		{
			std::string response = "324 Current channel's (" + chan.getName() + ") modes:\r\n- Invite only: ";
			if (chan.getInviteOnly())
				response += "true\r\n";
			else
				response += "false\r\n";
			response += "- Topic locked: ";
			if (chan.getTopicLocked())
				response += "true\r\n";
			else
				response += "false\r\n";
			response += "- Key: ";
			if (chan.getHasKey())
				response += "true\r\n";
			else
				response += "false\r\n";
			response += "- Users limit: ";
			if (chan.getLimitUser())
			{
				std::stringstream ss;
				ss << chan.getUserLimit();
				response += (" " + ss.str() + "\r\n");
			}
			else
				response += "0\r\n";
			response += "- Operators:\r\n";
			std::set<int>::iterator it;
			for (it = chan.getOperators().begin(); it != chan.getOperators().end(); it++)
				response += "\t\t- " + getNickname(*it) + "\r\n";

//			std::cout << "No mode" << std::endl;
//			std::stringstream response;

			//response << ":localhost 324 " << _clients[client_fd].nickname << chan.getName() << ":\r\n";
			sendToClient(client_fd, response);
			return true;
		}

		if (!chan.isMember(client_fd))
		{
			//std::string response = "442 " + channelOrUser + " MODE :You're not on that channel\r\n";

			std::stringstream response;

			response << ":localhost 442 " << _clients[client_fd].nickname << chan.getName() << " :You're not on that channel\r\n";
			sendToClient(client_fd, response.str());
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
								chan.setKey(key);
								std::stringstream response;
								response << ":" << _clients[client_fd].nickname
										<< " MODE " << chan.getName()
										<< " +t"
										<< ""
										<< "\r\n";
								broadcastToChannel(chan.getName(), response.str(), -1);
							}
						}
						else
						{
							chan.setKey("");
							std::stringstream response;
							response << ":" << _clients[client_fd].nickname
									<< " MODE " << chan.getName()
									<< " -t"
									<< ""
									<< "\r\n";
							broadcastToChannel(chan.getName(), response.str(), -1);
						}
						continue;
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
						if (opUser.empty())
						{
							std::string response = "MODE :Not enough argument to set/remove an user's channel operator privilege\r\n";
							sendToClient(client_fd, response);
							continue;
						}
						int fd = getFdByNickname(opUser);
						if (fd < 0)
						{
							std::string response = "401 " + opUser + " :No such nick\r\n";
							sendToClient(client_fd, response);
							continue;
						}
						if (!chan.isMember(fd))
						{
							std::string response = "441 " + opUser + " :They aren't on that channel\r\n";
							sendToClient(client_fd, response);
							continue;
						}
						if (add)
						{
							chan.getOperators().insert(fd);
							std::string response = "MODE :Added " + opUser + " as an channel operator for " + chan.getName() + "\r\n";
							sendToClient(client_fd, response);
						}
						else
						{
							chan.getOperators().erase(fd);
							std::string response = "MODE :Removed channel operator " + opUser + " from " + chan.getName() + "\r\n";
							sendToClient(client_fd, response);
						}
						continue;
					}
					default:
					{
						std::string response = "501 :Unknown MODE flag\r\n";
						sendToClient(client_fd, response);
						continue;
					}
				}
			}
			if(iss.eof())
				break ;
			iss >> modes;
		}
	}

	return true;
}
