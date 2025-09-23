//
//
//

#include "../includes/Server.hpp"

/*
/ This function must be called inside a try block.
*/
User& Server::getUserByFd(int fd) {
	for (
		UserListIterator it = this->users.begin() ;
		it != this->users.end() ;
		++it
	) {
		if (it->getFd() == fd) {
			return *it;
		}
	}
	throw std::runtime_error("User not found");
}

/*
/ This function must be called inside a try block.
*/
User& Server::getUserByNickname(const User& targetUser, const std::string& nickname) {
	for (
		UserListIterator it = this->users.begin() ;
		it != this->users.end() ;
		++it
	) {
		if (it->getNickname() == nickname) {
			return *it;
		}
	}
	std::string errorMsg = nickname + " :No such nick";
	sendNumericReply(&targetUser, ERR_NOSUCHNICK, errorMsg);
	throw std::runtime_error("User with nickname '" + nickname + "' not found");
}

bool Server::userExists(int fd) const {
	for (
		UserListConstIterator it = this->users.begin();
		it != this->users.end() ;
		++it
	) {
		if (it->getFd() == fd) {
			return true;
		}
	}
	return false;
}

bool Server::nicknameExists(const std::string& nickname) const {
	for (
		UserListConstIterator it = this->users.begin();
		it != this->users.end() ;
		++it
	) {
		if (it->getNickname() == nickname) {
			return true;
		}
	}
	return false;
}

void Server::registerUser(User &user) {
	if (user.getPassword().empty() ||
		user.getNickname().empty() ||
		user.getUsername().empty() ||
		user.getRealname().empty()) {
			return ;
	}
	if (!user.isRegistered()) {
		user.setRegistered(true); //passa para registrado

		std::cout << "🥳 Client " << user.getNickname()
			<< " fully registred!" << RESET << std::endl;
		user.setUserIdentifier();

		std::string welcomeUserMessage =
			" :Welcome to the " + this->name +
			" Network, " + user.getUserIdentifier();
		sendNumericReply(&user, RPL_WELCOME, welcomeUserMessage);

		std::string versionServerMessage =
			":Your host is " + this->name +
			" , running on version " + this->version;
		sendNumericReply(&user, RPL_YOURHOST, versionServerMessage);

		std::string formattedServerCreationTime = Parser::formatTimeStamp(this->serverCreationTime);
		std::string serverCreatedMessage =
			":This server was created " + formattedServerCreationTime;
		sendNumericReply(&user, RPL_CREATED, serverCreatedMessage);

		std::string userModes = REGISTRATION_MODE;
		std::string channelModes = INVITE_MODE + TOPIC_MODE + PASSWORD_MODE + OPERATOR_MODE + LIMIT_MODE;
		std::string serverInfoMessage = this->name + " " + this->version +
			" " + userModes +
			" " + channelModes;
		sendNumericReply(&user, RPL_MYINFO, serverInfoMessage);
	}
}

void Server::replyToUserWho(const User* askingUser, const User* targetUser) {
	if (!askingUser || !targetUser) {
		return ;
	}
	std::string askingUserChannel = askingUser->getChannels().size() <= 0
		? "*"
		: askingUser->getChannels().front()->getName();
	std::string hereFlag = "H";

	std::string userInfo = askingUserChannel
		+ " " + targetUser->getUsername()
		+ " " + targetUser->getIpAddress()
		+ " " + this->name + " " + targetUser->getNickname()
		+ " " + hereFlag
		+ " :0 " + targetUser->getRealname();

	sendNumericReply(askingUser, RPL_WHOREPLY, userInfo);
	std::string endReply = targetUser->getNickname() + " :End of WHO list";
	sendNumericReply(askingUser, RPL_ENDOFWHO, endReply);
}

void Server::clearUser(int fd) {
	for (PollIterator pIt = pollFds.begin(); pIt != pollFds.end(); ++pIt) {
		if (pIt->fd == fd) {
			pollFds.erase(pIt);
			break ;
		}
	}
	for (UserListIterator cIt = users.begin(); cIt != users.end(); ++cIt) {
		if (cIt->getFd() == fd) {
			users.erase(cIt);
			break;
		}
	}
	for (BufferMapIterator bIt = rawMessageBuffers.begin(); bIt != rawMessageBuffers.end(); ++bIt) {
		if (bIt->first == fd) {
			rawMessageBuffers.erase(bIt);
			break;
		}
	}
}

void Server::disconnectUser(int fd) {
	try {
		User& targetUser = getUserByFd(fd);
		targetUser.setPendingDisconnect(true);
	} catch (const std::exception& e) {
		std::cerr << "Disconnect: " << e.what() << std::endl;
	}
}
