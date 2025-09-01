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
User& Server::getUserByNickname(const std::string& nickname) {
	for (
		UserListIterator it = this->users.begin() ;
		it != this->users.end() ;
		++it
	) {
		if (it->getNickname() == nickname) {
			return *it;
		}
	}
	throw std::runtime_error("User with nickname '" + nickname + "' not found");
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

		// TODO Aqui devem estar os diferentes modes disponíveis do server
		std::string serverInfoMessage =
			"⚠️" + this->name + " " + this->version +
			" " + "<available user modes>" +
			" " + "<available channel modes>" +
			" " + "[<channel modes with a parameter>]";
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
