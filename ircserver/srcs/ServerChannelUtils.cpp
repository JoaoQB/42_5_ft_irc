//
//
//

#include "../includes/Server.hpp"

/*
/ This function must be called inside a try block.
*/
Channel& Server::getChannel(User& targetUser, const std::string& channelName) {
	for (
		ChannelListIterator it = this->channels.begin() ;
		it != this->channels.end() ;
		++it
	) {
		if (it->getName() == channelName) {
			return *it;
		}
	}
	std::string noSuchChannel = channelName
		+ " :No such channel";
	sendNumericReply(&targetUser, ERR_NOSUCHCHANNEL, noSuchChannel);
	throw std::runtime_error("Channel not found");
}

bool Server::channelExists(const std::string& channelName) const {
	for (
		ChannelListConstIterator it = this->channels.begin() ;
		it != this->channels.end() ;
		++it
	) {
		if (it->getName() == channelName) {
			return true;
		}
	}
	return false;
}

void Server::createChannel(
	User& creator,
	const std::string& channelName,
	const std::string& channelKey
) {
	Channel newChannel;
	this->channels.push_back(newChannel);
	Channel* joinedChannel = &this->channels.back();

	joinedChannel->setName(channelName);
	if (!channelKey.empty()) {
		joinedChannel->setPassword(channelKey);
	}
	joinedChannel->addUser(&creator);
	joinedChannel->addOperator(&creator);

	creator.addChannel(joinedChannel);

	sendJoinReplies(&creator, joinedChannel);
	std::cout << "Channel " << channelName << " created by user " << creator.getFd() << "\n";
}

void Server::addUserToChannel(
	User& targetUser,
	const std::string& channelName,
	const std::string& channelKey
) {
	Channel& targetChannel = getChannel(targetUser, channelName);
	if (targetChannel.hasUser(&targetUser)) {
		std::cerr << "User " << targetUser.getFd() << " is already in channel " << channelName << std::endl;
		return;
	}
	if (targetChannel.isFull()) {
		Parser::ft_error("channel full");
		std::string channelIsFull = targetChannel.getName()
			+ " :Cannot join channel (+l)";
		sendNumericReply(&targetUser, ERR_CHANNELISFULL, channelIsFull);
		return ;
	}
	if (targetChannel.isInviteOnly()) {
		Parser::ft_error("channel is invite only");
		std::string inviteOnly = targetChannel.getName()
			+ " :Cannot join channel (+i)";
		sendNumericReply(&targetUser, ERR_INVITEONLYCHAN, inviteOnly);
		return ;
	}
	if (targetChannel.requiresPassword() &&
		targetChannel.getPassword() != channelKey
	) {
		Parser::ft_error("channel password is incorrect");
		std::string badChannelKey = targetChannel.getName()
			+ " :Cannot join channel (+k)";
		sendNumericReply(&targetUser, ERR_BADCHANNELKEY, badChannelKey);
		return;
	}
	targetChannel.addUser(&targetUser);
	targetUser.addChannel(&targetChannel);
	sendJoinReplies(&targetUser, &targetChannel);
	std::cout << "User " << targetUser.getFd() << " added to channel " << channelName << std::endl;
}

void Server::sendJoinReplies(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	broadcastCommand(user, channel, "JOIN", channel->getName());
	sendChannelTopic(user, channel);
	sendChannelUsers(user, channel);
	sendChannelSetAt(user, channel);
}

void Server::broadcastCommand(
	const User* user,
	const Channel* channel,
	const std::string& command,
	const std::string& message
) {
	if (!user || !channel) {
		return ;
	}
	std::string commandMessage = ":" + user->getUserIdentifier()
		+ " " + command;

	if (!message.empty()) {
		commandMessage += " " + message;
	}

	for (
		UserVectorConstIterator it = channel->getUsers().begin();
		it != channel->getUsers().end();
		++it
	) {
		sendMessage((*it)->getFd(), commandMessage);
	}
}

void Server::sendChannelTopic(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	if (!channel->hasTopic()) {
		std::string noTopicMessage = channel->getName()
			+ " :" + "No topic is set";
		sendNumericReply(user, RPL_NOTOPIC, noTopicMessage);
		return;
	}
	std::string topicMessage = channel->getName()
		+ " :" + channel->getTopic();
	sendNumericReply(user, RPL_TOPIC, topicMessage);

	std::string topicWhoTimeMessage = channel->getName()
		+ " " + channel->getTopicSetter()
		+ " " + channel->getTopicCreationTime();
	sendNumericReply(user, RPL_TOPICWHOTIME, topicWhoTimeMessage);
}

void Server::sendChannelUsers(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	std::string channelUsers;
	for (
		UserVectorConstIterator it = channel->getUsers().begin();
		it != channel->getUsers().end();
		++it
	) {
		std::string isOperatorPrefix = channel->isOperator(*it) ? "@" : "";
		channelUsers += isOperatorPrefix + (*it)->getNickname() + " ";
	}
	std::string usersMessage = "= " + channel->getName()
		+ " :" + channelUsers;
	sendNumericReply(user, RPL_NAMREPLY, usersMessage);

	std::string endOfNames = channel->getName()
		+ " :End of /NAMES list";
	sendNumericReply(user, RPL_ENDOFNAMES, endOfNames);
}

void Server::sendChannelSetAt(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}

	std::string setAt = channel->getName()
		+ " " + channel->getCreationTime();
	sendNumericReply(user, RPL_CREATIONTIME, setAt);
}

void Server::replyToChannelWho(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	std::string channelUsers;
	for (
		UserVectorConstIterator it = channel->getUsers().begin();
		it != channel->getUsers().end();
		++it
	) {
		const User* targetUser = (*it);
		std::string userInfo = channel->getName()
			+ " " + targetUser->getUsername()
			+ " " + targetUser->getIpAddress()
			+ " " + this->name + " " + targetUser->getNickname()
			+ " :0 " + targetUser->getRealname();
		sendNumericReply(user, RPL_WHOREPLY, userInfo);
	}
	std::string endReply = channel->getName() + " :End of WHO list";
	sendNumericReply(user, RPL_ENDOFWHO, endReply);
}

/*
// This function can only be called on a copy of a user's <channel*> vector,
// because it modifies user->userChannels.
*/
void Server::partUserFromChannel(
	User* user, Channel* channel,
	bool quit,
	const std::string& quitReason
) {
	if (!user || !channel) {
		return ;
	}
	if (quit) {
		broadcastCommand(user, channel, "QUIT", quitReason);
	} else {
		broadcastCommand(user, channel, "PART", channel->getName());
	}
	channel->removeUser(user);
	user->removeChannel(channel);
	if (channel->isEmpty()) {
		this->removeChannel(channel);
		return;
	}
}

void Server::disconnectUserFromAllChannels(
	User* user,
	bool quit,
	const std::string& quitReason
) {
	if (!user) {
		return;
	}
	std::vector<Channel*> userChannelCopies = user->getChannels();
	for (
		ChannelVectorIterator channelIt = userChannelCopies.begin();
		channelIt != userChannelCopies.end();
		++channelIt
	) {
		debugPrintUsersAndChannels();
		partUserFromChannel(user, *channelIt, quit, quitReason);
	}
	user->getChannels().clear();
	debugPrintUsersAndChannels();
}

void Server::removeChannel(Channel* channel) {
	if (!channel || !channel->isEmpty()) {
		return;
	}

	for (std::list<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (&(*it) == channel) {
			std::cout << "Removing empty channel: " << it->getName() << std::endl;
			channels.erase(it);
			return;
		}
	}
}
