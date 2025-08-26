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
	, realname("jqueijo-")
	, username("jqueijo-")
	, nickname("jqueijo-")
	, password()
	, userChannels() {
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

bool User::hasChannel(const Channel* channel) const {
	if (!channel) return false;

	return std::find(userChannels.begin(), userChannels.end(), channel) != userChannels.end();
}

void User::addChannel(Channel* channel) {
	if (!channel || hasChannel(channel)) {
		return;
	}
	this->userChannels.push_back(channel);
}
