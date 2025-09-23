//
//
//

#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"

Channel::Channel()
	: name()
	, password()
	, channelCreationTime()
	, topic()
	, topicSetter()
	, topicCreationTime()
	, channelLimit(-1)
	, usersInChannel(0)
	, channelUsers()
	, channelOperators()
	, channelModes()
	, invitedUsers() {
	channelCreationTime = Parser::getTimestamp();
	channelModes.insert(TOPIC_MODE);
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
	return this->channelCreationTime;
}

int Channel::getUsersInChannel() const {
	return this->usersInChannel;
}

int Channel::getChannelLimit() const {
	return this->channelLimit;
}

const std::vector<User*>& Channel::getUsers() const {
	return this->channelUsers;
}

const std::vector<User*>& Channel::getOperators() const {
	return this->channelOperators;
}

const StringSet& Channel::getChannelModes() const {
	return this->channelModes;
}

StringSet& Channel::getChannelModes() {
	return this->channelModes;
}

bool Channel::isFull() const {
	return channelLimit == usersInChannel;
}

bool Channel::isEmpty() const {
	return this->usersInChannel <= 0;
}

bool Channel::requiresPassword() const {
	return std::find(channelModes.begin(), channelModes.end(), PASSWORD_MODE) != channelModes.end();
}

bool Channel::isInviteOnly() const {
	return std::find(channelModes.begin(), channelModes.end(), INVITE_MODE) != channelModes.end();
}

bool Channel::hasTopic() const {
	return !this->topic.empty();
}

bool Channel::isTopicProtected() const {
	return std::find(channelModes.begin(), channelModes.end(), TOPIC_MODE) != channelModes.end();
}

bool Channel::hasOperator() const {
	return !this->channelOperators.empty();
}

bool Channel::hasLimit() const {
	return this->channelLimit != -1;
}

bool Channel::hasUser(const User* user) const {
	if (!user) return false;

	bool hasUser = std::find(
		channelUsers.begin(),
		channelUsers.end(),
		user
	) != channelUsers.end();

	return hasUser;
}

bool Channel::isOperator(const User* user) const {
	if (!user) return false;

	bool isOperator = std::find(
		channelOperators.begin(),
		channelOperators.end(),
		user
	) != channelOperators.end();

	return isOperator;
}

bool Channel::isInvited(const User* user) const {
	if (!user) return false;

	bool isInvited = std::find(
		invitedUsers.begin(),
		invitedUsers.end(),
		user->getNickname()
	) != invitedUsers.end();

	return isInvited;
}

void Channel::setName(const std::string& channelName) {
	this->name = channelName;
}

void Channel::setPassword(const std::string& key) {
	this->password = key;
	this->channelModes.insert(PASSWORD_MODE);
}

void Channel::setTopic(const User* user, const std::string& message) {
	this->topic = message;
	this->topicSetter = user->getNickname();
	this->topicCreationTime = Parser::getTimestamp();
}

void Channel::setLimit(int limit) {
	this->channelLimit = limit;
	channelModes.insert(LIMIT_MODE);
}

void Channel::addUser(User* user) {
	if (!user || hasUser(user)) {
		return;
	}
	this->channelUsers.push_back(user);
	this->usersInChannel++;
}

void Channel::addOperator(User* user) {
	if (!user) {
		return;
	}
	if (!isOperator(user)) {
		this->channelOperators.push_back(user);
	}
}

void Channel::addInvitedUser(const User* user) {
	if (!user) {
		return;
	}
	this->invitedUsers.insert(invitedUsers.end(), user->getNickname());
}

void Channel::removeUser(Server& server, User* user) {
	if (!user) {
		return;
	}

	UserVectorIterator it = std::remove(
		channelUsers.begin(),
		channelUsers.end(),
		user
	);
	if (it != channelUsers.end()) {
		channelUsers.erase(it, channelUsers.end());
		this->usersInChannel--;
	}
	if (isOperator(user)) {
		removeOperator(server, user);
	}
	getUsers();
}

void Channel::removeOperator(Server& server, User* user) {
	if (!user) {
		return;
	}

	UserVectorIterator it = std::remove(
		channelOperators.begin(),
		channelOperators.end(),
		user
	);
	if (it != channelOperators.end()) {
		channelOperators.erase(it, channelOperators.end());
	}
	if (hasOperator()) {
		return;
	}
	for (
		UserVectorIterator it = channelUsers.begin();
		it != channelUsers.end();
		++it
	) {
		User* targetOperator = *it;
		if (!isOperator(targetOperator)) {
			addOperator(targetOperator);
			std::string modeCommand = getName() + " +o " + targetOperator->getNickname();
			server.broadcastCommand(server.name, this, "MODE", modeCommand);
			return;
		}
	}
}

void Channel::removeTopic() {
	this->topic = "";
	this->topicCreationTime = "";
	this->topicSetter = "";
}

void Channel::removePassword() {
	this->password = "";
	channelModes.erase(PASSWORD_MODE);
}

void Channel::removeLimit() {
	this->channelLimit = -1;
	channelModes.erase(LIMIT_MODE);
}

void Channel::removeInviteOnly() {
	invitedUsers.clear();
	channelModes.erase(INVITE_MODE);
}
