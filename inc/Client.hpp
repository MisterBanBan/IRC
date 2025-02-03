/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 13:30:34 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/03 13:30:34 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <set>

class Client
{
    public:
        int fd;
        std::string buffer_in;
        std::string buffer_out;
        std::string nickname;
        std::string user;
        std::string realname;
        bool is_authenticated;
		bool right_pass;
        std::string ip;
        int port;
        std::set<std::string> channels;
        std::string getNickname() const { return nickname; }
    private:
};

#endif