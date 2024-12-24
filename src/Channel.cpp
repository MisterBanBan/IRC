#include "Channel.hpp"

Channel::Channel(void) { }

Channel::Channel(const std::string &channelName) : _name(channelName)
{

}

Channel::Channel(const Channel &other)
{
    *this = &other;
}

~Channel::Channel(void) { }

Channel &Channel::operator=(const Channel &other)
{
    if (this != &other)
    {

    }
    return this;
}

