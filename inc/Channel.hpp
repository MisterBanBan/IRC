/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 09:14:22 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/29 09:14:26 by mbaron-t         ###   ########.fr       */
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
        bool isMember(int fd) const;
        void inviteUser(int fd);
        bool isInvited(int fd);
        std::string getTopic() const;
		std::string getName() const;
        void setTopic(const std::string &topic);
        bool inviteOnly; //+i
        bool topicLocked; //+t
        bool hasKey; //+k
        std::string key;
        bool limitUser; //+l
        int userLimit;

        std::set<int> operators; //+o
        
    private:
        std::string _name;
        std::set<int> _members;
        std::string _topics;
        std::set<int> _inviteUser;
};

#endif
