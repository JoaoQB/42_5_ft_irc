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
	, isInviteOnly(false)
	, isFull(false)
	, channelLimit(-1)
	, usersInChannel(0)
	, channelUsers() {
}

const std::string& Channel::getName() const {
	return this->name;
}

const std::string& Channel::getPassword() const {
	return this->password;
}

int Channel::channelIsFull() const {
	return this->isFull;
}

bool Channel::requiresPassword() const {
	return this->hasPassword;
}

void Channel::setName(const std::string& channelName) {
	this->name = channelName;
}

void Channel::setPassword(const std::string& key) {
	this->password = key;
	this->hasPassword = true;
}

void Channel::addUser(User user) {
	this->channelUsers.push_back(user);
	this->usersInChannel++;
	if (channelLimit == usersInChannel) {
		this->isFull = true;
	}
}
