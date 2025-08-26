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
	, channelCreationTime()
	, topic()
	, topicSetter()
	, topicCreationTime()
	, hasPassword(false)
	, inviteOnly(false)
	, full(false)
	, channelLimit(-1)
	, usersInChannel(0)
	, channelUsers()
	, channelOperators() {
	channelCreationTime = Parser::getTimestamp();
}

// Test printing channel topic
// Channel::Channel()
// 	: name()
// 	, password()
// 	, channelCreationTime()
// 	, topic("a contemporaneadade")
// 	, topicSetter("theBest!")
// 	, topicCreationTime()
// 	, hasPassword(false)
// 	, inviteOnly(false)
// 	, full(false)
// 	, channelLimit(-1)
// 	, usersInChannel(0)
// 	, channelUsers()
// 	, channelOperators() {
// 	channelCreationTime = Parser::getTimestamp();
// 	topicCreationTime = Parser::getTimestamp();
// }

void Channel::setName(const std::string& channelName) {
	this->name = channelName;
}

void Channel::setPassword(const std::string& key) {
	this->password = key;
	this->hasPassword = true;
}

void Channel::setTopic(const std::string& message) {
	this->topic = message;

}

const std::string& Channel::getName() const {
	return this->name;
}

const std::string& Channel::getPassword() const {
	return this->password;
}

const std::string& Channel::getTopic() const {
	return this->topic;
}

const std::string& Channel::getTopicSetter() const {
	return this->topicSetter;
}

const std::string& Channel::getTopicCreationTime() const {
	return this->topicCreationTime;
}

const std::string& Channel::getCreationTime() const {
	return this->topicCreationTime;
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

bool Channel::hasTopic() const {
	return !this->topic.empty();
}

bool Channel::isOperator(const User* user) const {
	if (!user) return false;

	return std::find(channelOperators.begin(), channelOperators.end(), user) != channelOperators.end();
}

const std::vector<User*>& Channel::getUsers() const {
	return this->channelUsers;
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

