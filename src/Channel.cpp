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

Channel::Channel(const std::string &channelName) : _name(channelName), _inviteOnly(false), _topicLocked(false), _hasKey(false), _limitUser(false), _userLimit(0)
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

bool Channel::isInvited(int fd) const
{
    return (_inviteUser.find(fd) != _inviteUser.end());
}

std::string Channel::getName() const
{
	return _name;
}

bool Channel::isOperator(int fd) const
{
	if (_operators.find(fd) == _operators.end())
		return false;
	return true;
}

int Channel::getUserLimit() const {
	return _userLimit;
}

std::set<int> &Channel::getOperators() {
	return _operators;
}

void Channel::setUserLimit(int limit) {
	_userLimit = limit;
}

std::string Channel::getKey() const {
	return _key;
}

void Channel::setKey(const std::string & key) {
	_key = key;
}

bool Channel::getHasKey() const {
	return _hasKey;
}

bool Channel::getTopicLocked() const {
	return _topicLocked;
}

bool Channel::getInviteOnly() const {
	return _inviteOnly;
}

bool Channel::getLimitUser() const {
	return _limitUser;
}

void Channel::setTopicLocked(bool active) {
	_topicLocked = active;
}

void Channel::setHasKey(bool active) {
	_hasKey = active;
}

void Channel::setInviteOnly(bool active) {
	_inviteOnly = active;
}

void Channel::setLimitUser(bool active) {
	_limitUser = active;
}