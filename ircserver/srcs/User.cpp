/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:16 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/06/27 12:48:00 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/User.hpp"

User::User()
	: fd(-1)
	, ipAddress()
	, realname()
	, username()
	, nickname()
	, password()
	, userChannels() {
	this->registered = false;
}

void User::setFd(int fd) {
	this->fd = fd;
}

void User::setIpAddress(const std::string& ipAddr) {
	ipAddress = ipAddr;
}

int User::getFd() const {
	return fd;
}

const std::string& User::getNickname() const {
	return nickname;
}

const std::string& User::getUsername() const {
	return username;
}

const std::string& User::getIpAddress() const {
	return ipAddress;
}

std::string User::getUserIdentifier() const {
	return nickname + "!" + username + "@" + ipAddress;
}

std::vector<Channel*>& User::getChannels() {
	return this->userChannels;
}

bool User::hasChannel(const Channel* channel) const {
	if (!channel) return false;

	bool hasChannel = std::find(
		userChannels.begin(),
		userChannels.end(),
		channel
	) != userChannels.end();

	// std::cout << "[Debug] Checking if has channel for: "
	// 	<< channel->getName()
	// 	<< " @ " << channel << std::endl;

	// Parser::debugPrintChannels(userChannels);
	// std::cout << "[Debug] is " << hasChannel << "\n";

	return hasChannel;
}

void User::addChannel(Channel* channel) {
	if (!channel || hasChannel(channel)) {
		return;
	}
	this->userChannels.push_back(channel);
}

void User::removeChannel(Channel* channel) {
	if (!channel) {
		return;
	}
	ChannelVectorIterator it = std::remove(
		this->userChannels.begin(),
		this->userChannels.end(),
		channel
	);
	if (it != this->userChannels.end()) {
		userChannels.erase(it, userChannels.end());
	}
}

std::string User::getRealName(void) {
	return this->realname;
}

void User::setRealName(std::string realname) {
	this->realname = realname;
}

std::string User::getUserName(void) {
	return this->username;
}

void User::setUserName(std::string username) {
	this->username = username;
}

std::string User::getNickName(void) {
	return this->nickname;
}

void User::setNickName(std::string nickname) {
	this->nickname = nickname;
}

std::string User::getPassword(void) {
	return this->password;
}

void User::setPassword(std::string password) {
	this->password = password;
}

bool User::isRegistered(void) {
	return this->registered;
}

void User::setRegistered(bool registred) {
	this->registered = registred;
}
