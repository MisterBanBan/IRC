/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 09:14:22 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/01/29 09:14:22 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(void) { }

Channel::Channel(const std::string &channelName) : _name(channelName)
{

}

Channel::Channel(const Channel &other)
{
    *this = other;
}

Channel::~Channel(void) { }

Channel &Channel::operator=(const Channel &other)
{
    if (this != &other)
    {
        this->_name = other._name;
        this->_members = other._members;
        this->_topics = other._topics;
    }
    return *this;
}

void Channel::addMember(int fd)
{
    _members.insert(fd);
}

void Channel::removeMember(int fd)
{
    _members.erase(fd);
}

bool Channel::isMember(int fd) const
{
    return (_members.find(fd) != _members.end());
}

const std::set<int>& Channel::getMembers() const
{
    return _members;
}

std::string Channel::getTopic() const
{
    return _topics;
}

void Channel::setTopic(const std::string &topic)
{
    this->_topics = topic;
}

void Channel::inviteUser(int fd)
{
    this->_inviteUser.insert(fd);
}

bool Channel::isInvited(int fd)
{
    return (_inviteUser.find(fd) != _inviteUser.end());
}

std::string Channel::getName() const {
	return _name;
}
