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
        std::string getTopic() const;
        void setTopic(const std::string &topic);
        
    private:
        std::string _name;
        std::set<int> _members;
        std::string _topics;
};

#endif
