/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 10:29:24 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/06 15:41:45 by afavier          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
#define REPLIES_HPP

#include "utils.hpp"

#define KICK(executer, channel, target, reason) (":" + executer + " KICK " + channel + " " + target + " :" + reason + "\r\n")
#define MODE(executer, channel, flag, arg) (":" + executer + " MODE " + channel + " " + flag + " :" + arg + "\r\n")
#define PART(executer, channel, reason) (":" + executer + " PART " + channel + " " + executer + " :" + reason + "\r\n")
#define TOPIC(executer, channel, topic) (":" + executer + " TOPIC " + channel + " :" + topic + "\r\n")
#define JOIN(executer, channel) (":" + executer + " JOIN " + channel + "\r\n")

#define RPL_WELCOME(nick) (":localhost 001 " + nick + " Welcome to the server, " + nick + "!\r\n")
#define RPL_NOTOPIC(channel) (":localhost 331 " + channel + " :No topic is set\r\n")											// 331
#define RPL_TOPIC(channel, topic) (":localhost 332 " + channel + " :" + topic + "\r\n")											// 332
#define RPL_INVITING(invited, channel) (":localhost 341 " + invited + " " + channel + "\r\n")									// 341
#define RPL_NAMREPLY(nick, channel, nicks) (":localhost 353 " + nick + " = " + channel + " :" + nicks + "\r\n")					// 353
#define RPL_ENDOFNAMES(channel) (":localhost 366 " + channel + " :End of /NAMES list\r\n")										// 366

#define ERR_NOSUCHNICK(nick) (":localhost 401 " + nick + " :No such nick\r\n")													// 401
#define ERR_NOSUCHCHANNEL(channel) (":localhost 403 " + channel + " :No such channel\r\n")										// 403
#define ERR_NORECIPIENT (":localhost 411 :No recipient given\r\n")																// 411
#define ERR_NOTEXTTOSEND (":localhost 412 :No text to send\r\n")																// 412
#define ERR_UNKNOWNCOMMAND(command) (":localhost 421 " + command + " :Unknown command\r\n")										// 421
#define ERR_NONICKNAMEGIVEN (":localhost 431 :No nickname given\r\n")															// 431
#define ERR_ERRONEUSNICKNAME(nick) (":localhost 432 " + nick + " :Erroneus nickname\r\n")										// 432
#define ERR_NICKNAMEINUSE(nick) (":localhost 433 " + nick + " :Nickname is already in use\r\n")									// 433
#define ERR_USERNOTINCHANNEL(nick, channel) (":localhost 441 " + nick + " " + channel + " :They aren't on that channel\r\n")	// 441
#define ERR_NOTONCHANNEL(channel) (":localhost 442 " + channel + " :You're not on that channel\r\n")							// 442
#define ERR_USERONCHANNEL(nick, channel) (":localhost 443 " + nick + " " + channel + " :is already on channel\r\n")				// 443
#define ERR_NOTREGISTERED (":localhost 451 :You have not registered\r\n")														// 451
#define ERR_NEEDMOREPARAMS(command) (":localhost 461 " + std::string(command) + " :Not enough parameters\r\n")					// 461
#define ERR_ALREADYREGISTERED (":localhost 462 :You may not reregister\r\n")													// 462
#define ERR_PASSWDMISMATCH (":localhost 464 :Password incorrect\r\n")															// 464
#define ERR_CHANNELISFULL(channel) (":localhost 471 " + channel + " :Cannot join channel (+l)\r\n")								// 471
#define ERR_INVITEONLYCHAN(channel) (":localhost 473 " + channel + " :Cannot join channel (+i)\r\n")							// 473
#define ERR_BADCHANNELKEY(channel) (":localhost 475 " + channel + " :Cannot join channel (+k)\r\n")								// 475
#define ERR_CHANOPRIVSNEEDED(channel) (":localhost 482 " + channel + " :You're not a channel operator\r\n")						// 482
#define ERR_UMODEUNKNOWNFLAG(flag) (":localhost 501 " + flag + " :Unknown MODE flag\r\n")										// 501
#define ERR_USERSDONTMATCH
#define ERR_INVALIDMODEPARAM(channel, arg, desc) (":localhost 696 " + channel + " " + arg + " :" + desc + "\r\n")				// 696


#endif
