/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 09:14:22 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/30 14:39:35 by mbaron-t         ###   ########.fr       */
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
        const std::set<int>& getMembers() const;
        void addMember(int fd);
        void removeMember(int fd);
		void inviteUser(int fd);

        bool isMember(int fd) const;
        bool isInvited(int fd) const;
		bool isOperator(int fd) const;

        std::string getTopic() const;
		std::string getName() const;
		std::string	getKey() const;
		int			getUserLimit() const;

		std::set<int> & getOperators();

		bool getHasKey() const;
		bool getTopicLocked() const;
		bool getInviteOnly() const;
		bool getLimitUser() const;

		void setTopicLocked(bool active);
		void setHasKey(bool active);
		void setInviteOnly(bool active);
		void setLimitUser(bool active);

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
		bool		_hasKey; //+k
		std::string	_key;
		bool		_limitUser; //+l
		int			_userLimit;

		std::set<int> _operators; //+o
};

#endif
