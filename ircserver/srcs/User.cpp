//
//
//

#include "../includes/User.hpp"

User::User()
	: fd(-1)
	, ipAddress()
	, realname()
	, username()
	, nickname()
	, password()
	, userIdentifier()
	, registered(false)
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

const std::string& User::getIpAddress() const {
	return this->ipAddress;
}

const std::string& User::getRealname() const {
	return this->realname;
}

const std::string& User::getNickname() const {
	return nickname;
}

const std::string& User::getUsername() const {
	return username;
}

const std::string& User::getPassword() const {
	return this->password;
}

const std::string& User::getUserIdentifier() const {
	return this->userIdentifier;
}

bool User::isRegistered(void) const {
	return this->registered;
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

std::vector<Channel*>& User::getChannels() {
	return this->userChannels;
}

void User::setRealname(std::string realname) {
	this->realname = realname;
}

void User::setUsername(std::string username) {
	this->username = username;
}

void User::setNickname(std::string nickname) {
	this->nickname = nickname;
}

void User::setPassword(std::string password) {
	this->password = password;
}

void User::setRegistered(bool registered) {
	this->registered = registered;
}

void User::setUserIdentifier() {
	this->userIdentifier = nickname + "!" + username + "@" + ipAddress;
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
