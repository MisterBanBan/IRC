/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 09:14:22 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/07 11:27:53 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <iostream>

class Channel
{

    public:
        Channel(void);
        Channel(const std::string &channelName);
        Channel(const Channel &other);
        ~Channel(void);
        Channel &operator=(const Channel &other);

        void addMember(int fd);
        void removeMember(int fd);
		void inviteUser(int fd);

        bool isMember(int fd) const;
        bool isInvited(int fd) const;
		bool isOperator(int fd) const;

        std::string getTopic() const;
		std::string getName() const;
		int			getUserLimit() const;

		std::set<int> & getOperators();
		std::set<int> & getMembers();

		bool hasKey() const;
		bool hasUserLimit() const;
		bool isTopicLocked() const;
		bool isInviteOnly() const;
		bool isValidKey(const std::string & key) const;

		void setTopicLocked(bool active);
		void setInviteOnly(bool active);

		void setUserLimit(int limit);
		void setKey(const std::string & key);
		void setTopic(const std::string &topic);
        
    private:
        std::string _name;
        std::set<int> _members;
        std::string _topics;
        std::set<int> _inviteUser;

		bool		_inviteOnly; //+i
		bool		_topicLocked; //+t
		std::string	_hashKey; // +k
		int			_userLimit; //+l

		std::set<int> _operators; //+o
};

#endif
