/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:50:18 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/05 11:29:34 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::mode(std::istringstream &iss, int client_fd) {
	std::string channelOrUser, modes;
	iss >> channelOrUser >> modes;

	if (!_clients[client_fd].isAuthenticated())
	{
		sendToClient(client_fd, ERR_NOTREGISTERED);
		return false;
	}

	if (channelOrUser.empty())
	{
		sendToClient(client_fd, ERR_NEEDMOREPARAMS("MODE"));
		return true;
	}

	if (channelOrUser[0] == '#')
	{
		if (_channels.find(channelOrUser) == _channels.end())
		{
			sendToClient(client_fd, ERR_NOSUCHCHANNEL(channelOrUser));
			return true;
		}
		Channel &chan = _channels[channelOrUser];

		if (modes.empty())
		{
			std::string response = "324 Current channel's (" + chan.getName() + ") modes:\r\n- Invite only: ";
			if (chan.isInviteOnly())
				response += "true\r\n";
			else
				response += "false\r\n";
			response += "- Topic locked: ";
			if (chan.isTopicLocked())
				response += "true\r\n";
			else
				response += "false\r\n";
			response += "- Key: ";
			if (chan.hasKey())
				response += "true\r\n";
			else
				response += "false\r\n";
			response += "- Users limit: ";
			if (chan.hasLimitUser())
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
			sendToClient(client_fd, ERR_NOTONCHANNEL(chan.getName()));
			return false;
		}

		if (!chan.isOperator(client_fd))
		{
			sendToClient(client_fd, ERR_CHANOPRIVSNEEDED(chan.getName()));
			return false;
		}

		bool add = false;

		while (!modes.empty())
		{
			for (size_t i = 0; i < modes.size(); i++)
			{
				if (modes[i] == '-')
					add = false;
				if (modes[i] == '+')
					add = true;
				if (modes[i] != '+' && modes[i] != '-' && i == 0)
				{
					sendToClient(client_fd, ERR_INVALIDMODEPARAM(chan.getName(), modes[i], "Modes needs to start with - or +"));
					break;
				}
				if (modes[i] == '+' || modes[i] == '-')
					continue;

				switch(modes[i])
				{
					case 'i':
						chan.setInviteOnly(add);
						if (add)
							broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "+i", ""), -1);
						else
							broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "-i", ""), -1);
						break;
					case 't':
						chan.setTopicLocked(add);
						if (add)
							broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "+t", ""), -1);
						else
							broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "-t", ""), -1);
						break;
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
								broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "+k", "<secret>"), client_fd);
								sendToClient(client_fd, MODE(_clients[client_fd].getNickname(), chan.getName(), "+k", key));
							}
							else
								sendToClient(client_fd, ERR_NEEDMOREPARAMS("MODE +k"));
						}
						else
						{
							chan.setKey("");
							broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "-k", ""), -1);
						}
						break;
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
									sendToClient(client_fd, ERR_INVALIDMODEPARAM(chan.getName(), "+l", "Needs to be a number and greater than 0"));
									break;
								}
								chan.setUserLimit(nb);
								broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "+l",
																			to_string(nb)), -1);
							}
							else
								sendToClient(client_fd, ERR_NEEDMOREPARAMS("MODE +l"));
						}
						else
						{
							chan.setUserLimit(0);
							broadcastToChannel(chan.getName(), MODE(_clients[client_fd].getNickname(), chan.getName(), "-l", ""), -1);
						}
						break;
					}
					case 'o':
					{
						std::string opUser;
						if (!iss.eof())
							iss >> opUser;
						if (opUser.empty())
						{
							sendToClient(client_fd, ERR_NEEDMOREPARAMS("MODE +o"));
							break;
						}
						int fd = getFdByNickname(opUser);
						if (fd < 0)
						{
							sendToClient(client_fd, ERR_NOSUCHNICK(opUser));
							break;
						}
						if (!chan.isMember(fd))
						{
							sendToClient(client_fd, ERR_USERNOTINCHANNEL(opUser, chan.getName()));
							break;
						}
						if (add)
						{
							chan.getOperators().insert(fd);
							sendToClient(client_fd, MODE(_clients[client_fd].getNickname(), chan.getName(), "+o", opUser));
						}
						else
						{
							chan.getOperators().erase(fd);
							sendToClient(client_fd, MODE(_clients[client_fd].getNickname(), chan.getName(), "-o", opUser));
						}
						break;
					}
					default:
					{
						sendToClient(client_fd, ERR_UMODEUNKNOWNFLAG(to_string(modes[i])));
						break;
					}
				}
			}

			iss >> modes;
			if(iss.eof())
				break ;
		}
	}

	return true;
}
