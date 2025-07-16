/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:26:10 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 10:11:17 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel()
	: name()
	, password()
	, topic()
	, hasPassword(false)
	, isInviteOnly(false)
	, usersInChannel(0)
	, channelLimit(-1)
	, isFull(false)
	, channelUsers() {
}

Channel::Channel(const std::string& channelName)
	: name(channelName)
	, password()
	, topic()
	, hasPassword(false)
	, isInviteOnly(false)
	, usersInChannel(0)
	, channelLimit(-1)
	, isFull(false)
	, channelUsers() {
}

Channel::Channel(const std::string& channelName, const std::string& key)
	: name(channelName)
	, password(key)
	, topic()
	, hasPassword(true)
	, isInviteOnly(false)
	, usersInChannel(0)
	, channelLimit(-1)
	, isFull(false)
	, channelUsers() {
}

const std::string& Channel::getName() const {
	return this->name;
}

int Channel::channelIsFull() const {
	return usersInChannel == channelLimit;
}

bool Channel::requiresPassword() const {
	return this->hasPassword;
}
