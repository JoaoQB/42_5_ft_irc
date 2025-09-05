//
//
//

#include "../includes/Channel.hpp"
#include "../includes/ChannelConstants.hpp"

Channel::Channel()
	: name()
	, password()
	, channelCreationTime()
	, topic()
	, topicSetter()
	, topicCreationTime()
	, hasPassword(false)
	, full(false)
	, channelLimit(-1)
	, usersInChannel(0)
	, channelUsers()
	, channelOperators()
	, channelModes() {
	channelCreationTime = Parser::getTimestamp();
	channelModes.push_back(TOPIC_MODE);
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

const std::vector<User*>& Channel::getUsers() const {
	return this->channelUsers;
}

const std::vector<User*>& Channel::getOperators() const {
	return this->channelOperators;
}

const StringVector& Channel::getChannelModes() const {
	return this->channelModes;
}

bool Channel::isFull() const {
	return this->full;
}

bool Channel::isEmpty() const {
	return this->usersInChannel <= 0;
}

bool Channel::requiresPassword() const {
	return this->hasPassword;
}

bool Channel::isInviteOnly() const {
	return std::find(channelModes.begin(), channelModes.end(), INVITE_MODE) != channelModes.end();
}

bool Channel::isOperator(const User* user) const {
	if (!user) return false;

	bool isOperator = std::find(
		channelOperators.begin(),
		channelOperators.end(), user) != channelOperators.end();

		// std::cout << "[Debug] Checking operator for: "
		// 	<< user->getUserIdentifier()
		// 	<< " @ " << user << std::endl;
		// Parser::debugPrintUsers(channelOperators);
		// std::cout << "[Debug] is " << isOperator << "\n";

		return isOperator;
}

bool Channel::hasTopic() const {
	return !this->topic.empty();
}

bool Channel::isTopicProtected() const {
	return std::find(channelModes.begin(), channelModes.end(), TOPIC_MODE) != channelModes.end();
}

bool Channel::hasNoOperator() const {
	return this->channelOperators.empty();
}

bool Channel::hasUser(const User* user) const {
	if (!user) return false;

	bool hasUser = std::find(
		channelUsers.begin(),
		channelUsers.end(),
		user
	) != channelUsers.end();

	// std::cout << "[Debug] Checking if has user for: "
	// 	<< user->getUserIdentifier()
	// 	<< " @ " << user << std::endl;

	// Parser::debugPrintUsers(channelUsers);
	// std::cout << "[Debug] is " << hasUser << "\n";

	return hasUser;
}

void Channel::setName(const std::string& channelName) {
	this->name = channelName;
}

void Channel::setPassword(const std::string& key) {
	this->password = key;
	this->hasPassword = true;
	this->channelModes.push_back(PASSWORD_MODE);
}

void Channel::setTopic(const User* user, const std::string& message) {
	this->topic = message;
	this->topicSetter = user->getNickname();
	this->topicCreationTime = Parser::getTimestamp();
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

void Channel::removeUser(User* user) {
	if (!user) return;

	UserVectorIterator it = std::remove(
		channelUsers.begin(),
		channelUsers.end(),
		user
	);
	if (it != channelUsers.end()) {
		channelUsers.erase(it, channelUsers.end());
		this->usersInChannel--;
	}
	it = std::remove(
		channelOperators.begin(),
		channelOperators.end(),
		user
	);
	if (it != channelOperators.end()) {
		channelOperators.erase(it, channelOperators.end());
	}
}

void Channel::deleteTopic() {
	this->topic = "";
	this->topicCreationTime = "";
	this->topicSetter = "";
}

