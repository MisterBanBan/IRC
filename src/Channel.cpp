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

