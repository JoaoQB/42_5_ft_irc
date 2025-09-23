//
//
//

#include "../includes/Server.hpp"

void Server::replyToChannelMode(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	std::string modes;
	const StringSet& channelModes = channel->getChannelModes();
	for (
		StringSet::const_iterator it = channelModes.begin();
		it != channelModes.end();
		++it
	) {
		modes.insert(0, *it);
	}

	std::ostringstream oss;
	if (channel->hasLimit()) {
		oss << " " << channel->getChannelLimit();
	} else {
		oss << "";
	}
	std::string modeStr = !modes.empty() ? (" +" + modes) : "";
	std::string channelLimit = oss.str();

	std::string replyMessage = channel->getName()
		+ modeStr
		+ channelLimit;

	sendNumericReply(user, RPL_CHANNELMODEIS, replyMessage);

	std::string setAt = channel->getName()
		+ " " + channel->getCreationTime();
	sendNumericReply(user, RPL_CREATIONTIME, setAt);
	return;
}

void Server::setChannelMode(
	const User* user,
	Channel* channel,
	const std::string& parameters
) {
	if (!user || !channel) {
		return ;
	}
	if (!channel->isOperator(user)) {
		sendNumericReply(
			user,
			ERR_CHANOPRIVSNEEDED,
			channel->getName() + " :You're not channel operator"
		);
		return;
	}
	const std::string modeString = Parser::extractFirstParam(parameters);
	const std::string modeArguments = Parser::extractFromSecondParam(parameters);
	// std::cout << "[DEBUG] modeString: " << modeString << std::endl;
	// std::cout << "[DEBUG] modeArguments: " << modeArguments << std::endl;
	// Protection check, should never be true
	if (modeString.empty()) {
		return;
	}
	char currentSign = *modeString.begin();
	if (!Parser::isValidModeSign(currentSign)) {
		sendNumericReply(user, ERR_UNKNOWNCOMMAND, "MODE :Missing sign (+/-)");
		return;
	}
	StringVector validFlags;
	std::string invalidFlags;
	for (StringSizeT i = 1; i < modeString.size(); ++i) {
		const char flag = modeString[i];
		if (flag == '-' || flag == '+') {
			currentSign = flag;
			continue;
		}
		if (Parser::isValidChannelMode(flag)) {
			std::string currentFlag;
			currentFlag += currentSign;
			currentFlag += flag;
			validFlags.insert(validFlags.end(), currentFlag);
		} else {
			invalidFlags.push_back(flag);
		}
	}
	// if (!validFlags.empty()) {
	// 	// Parser::debugPrintVector(validFlags);
	// }
	StringMap modesWithParams = Parser::mapModesWithParams(
		validFlags,
		modeArguments
	);
	setAndBroadcastModes(user, channel, modesWithParams);
	if (!invalidFlags.empty()) {
		// std::cout << "[DEBUG] Unknown flags: " << invalidFlags << std::endl;
		sendNumericReply(
			user,
			ERR_UNKNOWNMODE,
			invalidFlags + " :is unknown mode char to me"
		);
	}
}

void Server::setAndBroadcastModes(
	const User* user,
	Channel* channel,
	const StringMap& modesWithParams
) {
	if (!user || !channel) {
		return;
	}
	std::string appliedModes;
	std::string currentSign;
	StringVector params;

	for (StringMapConstIterator it = modesWithParams.begin();
		it != modesWithParams.end();
		++it
	) {
		const std::string& mode = it->first;
		const std::string& param = it->second;

		if (!setMode(user, channel, mode, param)) {
			continue;
		}
		const std::string sign = mode.substr(0, 1);
		const std::string flag = mode.substr(1, 1);
		if (currentSign == sign) {
			appliedModes += flag;
		} else {
			appliedModes += mode;
			currentSign = sign;
		}
		if (Parser::modeNeedsParam(mode)) {
			params.insert(params.end(), param);
		}
	}
	if (appliedModes.empty()) {
		return;
	}
	std::ostringstream broadcastMessage;
	broadcastMessage << channel->getName() << " " << appliedModes;
	for (size_t i = 0; i < params.size(); ++i) {
		broadcastMessage << " " << params[i];
	}
	broadcastCommand(user->getUserIdentifier(), channel, "MODE", broadcastMessage.str());
}

// TODO refactor?
bool Server::setMode(
	const User* user,
	Channel* channel,
	const std::string& mode,
	const std::string& param
) {
	if (!channel || !user) {
		return false;
	}
	if (mode.size() != 2 || (Parser::modeNeedsParam(mode) && param.empty())) {
		return false;
	}
	const char sign = mode[0];
	const std::string flag = mode.substr(1);

	StringSet& channelModes = channel->getChannelModes();

	if (flag == OPERATOR_MODE) {
		// std::cout << "debug1\n";
		return handleOperatorMode(user, channel, sign, param);
	}
	if (sign == '+' && (flag == INVITE_MODE || flag == TOPIC_MODE)) {
		if (channelModes.find(flag) == channelModes.end()) {
			channelModes.insert(flag);
		}
		return true;
	}
	if (sign == '-' && flag == INVITE_MODE) {
		channel->removeInviteOnly();
		return true;
	}
	if (sign == '-' && flag == TOPIC_MODE) {
		channelModes.erase(flag);
		return true;
	}
	if (sign == '+' && flag == PASSWORD_MODE) {
		channel->setPassword(param);
		return true;
	}
	if (sign == '-' && flag == PASSWORD_MODE) {
		channel->removePassword();
		return true;
	}
	if (sign == '+' && flag == LIMIT_MODE) {
		int limit = 0;
		if (!Parser::stringToInt(param, limit) || limit <= 0) {
			return false;
		}
		channel->setLimit(limit);
		return true;
	}
	if (sign == '-' && flag == LIMIT_MODE) {
		channel->removeLimit();
		return true;
	}
	return false;
}

bool Server::handleOperatorMode(
	const User* user,
	Channel* channel,
	const char& sign,
	const std::string& param
) {
	if (!user || !channel ||
		!Parser::isValidModeSign(sign) ||
		param.empty()
	) {
		// std::cout << sign << "\n";
		// std::cout << param << "\n";
		// std::cout << "debug false\n";
		return false;
	}
	// std::cout << "debug2\n";
	try {
		// std::cout << "debug3\n";
		User& targetUser = getUserByNickname(*user, param);
		if (sign == '-') {
			if (channel->isOperator(&targetUser)) {
				channel->removeOperator(*this, &targetUser);
			}
			return true;
		}
		channel->addOperator(&targetUser);
		return true;
	} catch (const std::exception& e) {
		std::cerr << "MODE: " << e.what() << std::endl;
		return false;
	}
}
