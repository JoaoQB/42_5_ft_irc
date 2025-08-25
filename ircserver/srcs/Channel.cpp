/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:26:10 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:22:35 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel()
	: name()
	, password()
	, topic()
	, hasPassword(false)
	, inviteOnly(false)
	, full(false)
	, channelLimit(-1)
	, usersInChannel(0)
	, channelUsers()
	, channelOperators() {
}

const std::string& Channel::getName() const {
	return this->name;
}

const std::string& Channel::getPassword() const {
	return this->password;
}

bool Channel::isFull() const {
	return this->full;
}

bool Channel::requiresPassword() const {
	return this->hasPassword;
}

bool Channel::isInviteOnly() const {
	return this->inviteOnly;
}

void Channel::setName(const std::string& channelName) {
	this->name = channelName;
}

void Channel::setPassword(const std::string& key) {
	this->password = key;
	this->hasPassword = true;
}

void Channel::addUser(User* user) {
	if (!user || hasUser(user)) {
		return;
	}
	this->channelUsers.push_back(user);
	this->usersInChannel++;
	if (channelLimit == usersInChannel) {
		this->full = true;
	}
}

void Channel::addOperator(User* user) {
	if (!user) {
		return;
	}
	this->channelOperators.push_back(user);
}

bool Channel::hasUser(const User* user) const {
	if (!user) return false;

	return std::find(channelUsers.begin(), channelUsers.end(), user) != channelUsers.end();
}

