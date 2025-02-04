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
	Client();
	Client(int fd);
	~Client();

	int						getFd() const;
	std::string&			getBufferIn();
	std::string&			getBufferOut();
	std::string				getNickname() const;
	std::string				getUsername() const;
	std::string				getRealname() const;
	bool					isAuthenticated() const;
	bool					getRightPass() const;
	std::set<std::string>&	getChannels();

	void		setNickname(const std::string & nickname);
	void		setUsername(const std::string & username);
	void		setRealname(const std::string & realname);
	void		setAuthenticated(bool active);
	void		setRightPass(bool active);

private:
	int						_fd;
	std::string 			_buffer_in;
	std::string 			_buffer_out;
	std::string 			_nickname;
	std::string 			_username;
	std::string 			_realname;
	bool					_is_authenticated;
	bool					_right_pass;
	std::set<std::string>	_channels;
};

#endif