/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbaron-t <mbaron-t@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 13:42:30 by mbaron-t          #+#    #+#             */
/*   Updated: 2025/02/04 13:42:30 by mbaron-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() {}

Client::Client(int fd): _fd(fd), _is_authenticated(false), _right_pass(false) {}

Client::~Client() {}

void Client::setNickname(const std::string &nickname) {
	_nickname = nickname;
}

void Client::setUsername(const std::string &username) {
	_username = username;
}

void Client::setRealname(const std::string &realname) {
	_realname = realname;
}

void Client::setAuthenticated(bool active) {
	_is_authenticated = active;
}

void Client::setRightPass(bool active) {
	_right_pass = active;
}


int Client::getFd() const {
	return _fd;
}

std::string &Client::getBufferIn() {
	return _buffer_in;
}

std::string &Client::getBufferOut() {
	return _buffer_out;
}

std::string Client::getNickname() const {
	return _nickname;
}

std::string Client::getUsername() const {
	return _username;
}

std::string Client::getRealname() const {
	return _realname;
}

bool Client::isAuthenticated() const {
	return _is_authenticated;
}

bool Client::getRightPass() const {
	return _right_pass;
}

std::set <std::string> &Client::getChannels() {
	return _channels;
}
