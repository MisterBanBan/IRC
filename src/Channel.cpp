/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 09:14:22 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/07 11:30:09 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <crypt.h>

Channel::Channel(void) {
}

Channel::Channel(const std::string &channelName) : _name(channelName), _inviteOnly(false), _topicLocked(false), _userLimit(0)
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
		this->_inviteOnly = false;
		this->_topicLocked = false;
		this->_userLimit = 0;
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

void Channel::inviteUser(int fd)
{
	this->_inviteUser.insert(fd);
}

bool Channel::isMember(int fd) const
{
    return (_members.find(fd) != _members.end());
}

bool Channel::isInvited(int fd) const
{
	return (_inviteUser.find(fd) != _inviteUser.end());
}

bool Channel::isOperator(int fd) const
{
	if (_operators.find(fd) == _operators.end())
		return false;
	return true;
}

std::string Channel::getName() const
{
	return _name;
}

std::string Channel::getTopic() const
{
	return _topics;
}

int Channel::getUserLimit() const {
	return _userLimit;
}

std::set<int>& Channel::getMembers()
{
    return _members;
}

std::set<int> &Channel::getOperators() {
	return _operators;
}

bool Channel::hasKey() const {
	if (!_hashKey.empty())
		return true;
	return false;
}

bool Channel::hasUserLimit() const {
	if (_userLimit > 0)
		return true;
	return false;
}

bool Channel::isTopicLocked() const {
	return _topicLocked;
}

bool Channel::isInviteOnly() const {
	return _inviteOnly;
}

bool Channel::isValidKey(const std::string &key) const {
	std::string hash = crypt(key.c_str(), "$6$RNEuivJ08k");

	if (hash == _hashKey)
		return true;
	return false;
}

void Channel::setTopicLocked(bool active) {
	_topicLocked = active;
}

void Channel::setInviteOnly(bool active) {
	_inviteOnly = active;
}

void Channel::setUserLimit(int limit) {
	_userLimit = limit;
}

void Channel::setKey(const std::string & key) {
	_hashKey = crypt(key.c_str(), "$6$RNEuivJ08k");
}

void Channel::setTopic(const std::string &topic)
{
    this->_topics = topic;
}
