/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afavier <afavier@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 10:29:24 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/07 15:26:15 by mbaron-t         ###   ########.fr       */
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

#define RPL_WELCOME(nick) (":localhost 001 " + nick + " Welcome to the server, " + nick + "!\r\n")								// 001
#define RPL_ENDOFWHO(nick, mask) (":localhost 315 " + nick + " " + mask + " :End of WHO list\r\n")								// 315
#define RPL_NOTOPIC(nick, channel) (":localhost 331 " + nick + " " + channel + " :No topic is set\r\n")											// 331
#define RPL_TOPIC(nick, channel, topic) (":localhost 332 " + nick + " " + channel + " :" + topic + "\r\n")											// 332
#define RPL_INVITING(nick, invited, channel) (":localhost 341 " + nick + " " + invited + " " + channel + "\r\n")									// 341
#define RPL_WHOREPLY(sender, nick, channel, user, host, server, prefix, realname) (":localhost 352 " + sender + " " + nick + " " + channel + " " + user + " " + host + " " + server + " " + nick + " H" + prefix + " :" + realname + "\r\n")
#define RPL_NAMREPLY(nick, channel, nicks) (":localhost 353 " + nick + " = " + channel + " :" + nicks + "\r\n")					// 353
#define RPL_ENDOFNAMES(nick, channel) (":localhost 366 " + nick + " " + channel + " :End of /NAMES list\r\n")										// 366

#define ERR_NOSUCHNICK(nick, target) (":localhost 401 " + nick + " " + target + " :No such nick\r\n")													// 401
#define ERR_NOSUCHCHANNEL(nick, channel) (":localhost 403 " + nick + " " + channel + " :No such channel\r\n")										// 403
#define ERR_NORECIPIENT(nick) (":localhost 411 " + nick + " :No recipient given\r\n")																// 411
#define ERR_NOTEXTTOSEND(nick) (":localhost 412 " + nick + " :No text to send\r\n")																// 412
#define ERR_UNKNOWNCOMMAND(nick, command) (":localhost 421 " + nick + " " + command + " :Unknown command\r\n")										// 421
#define ERR_NONICKNAMEGIVEN(nick) (":localhost 431 " + nick + " :No nickname given\r\n")															// 431
#define ERR_ERRONEUSNICKNAME(sender, nick) (":localhost 432 " + sender + " " + nick + " :Erroneus nickname\r\n")										// 432
#define ERR_NICKNAMEINUSE(sender, nick) (":localhost 433 " + sender + " " + nick + " :Nickname is already in use\r\n")									// 433
#define ERR_USERNOTINCHANNEL(nick, target, channel) (":localhost 441 " + nick + " " + target + " " + channel + " :They aren't on that channel\r\n")	// 441
#define ERR_NOTONCHANNEL(nick, channel) (":localhost 442 " + nick + " " + channel + " :You're not on that channel\r\n")							// 442
#define ERR_USERONCHANNEL(nick, target, channel) (":localhost 443 " + nick + " " + target + " " + channel + " :is already on channel\r\n")				// 443
#define ERR_NOTREGISTERED(nick) (":localhost 451 " + nick + " :You have not registered\r\n")														// 451
#define ERR_NEEDMOREPARAMS(nick, command) (":localhost 461 " + nick + " " + std::string(command) + " :Not enough parameters\r\n")					// 461
#define ERR_ALREADYREGISTERED(nick) (":localhost 462 " + nick + " :You may not reregister\r\n")													// 462
#define ERR_PASSWDMISMATCH(nick) (":localhost 464 " + nick + " :Password incorrect\r\n")															// 464
#define ERR_CHANNELISFULL(nick, channel) (":localhost 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")								// 471
#define ERR_INVITEONLYCHAN(nick, channel) (":localhost 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n")							// 473
#define ERR_BADCHANNELKEY(nick, channel) (":localhost 475 " + nick + " " + channel + " :Cannot join channel (+k)\r\n")								// 475
#define ERR_CHANOPRIVSNEEDED(nick, channel) (":localhost 482 " + nick + " " + channel + " :You're not a channel operator\r\n")						// 482
#define ERR_UMODEUNKNOWNFLAG(nick, flag) (":localhost 501 " + nick + " " + flag + " :Unknown MODE flag\r\n")										// 501
#define ERR_INVALIDMODEPARAM(nick, channel, arg, desc) (":localhost 696 " + nick + " " + channel + " " + arg + " :" + desc + "\r\n")				// 696


#endif
