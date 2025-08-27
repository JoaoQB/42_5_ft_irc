/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:59:51 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:30:18 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include "../includes/User.hpp"

Parser::Parser() {
}

std::string Parser::extractCommand(const std::string& rawMessage) {
	StringSizeT firstSpace = rawMessage.find(' ');
	if (firstSpace == std::string::npos) {
		return rawMessage;
	}
	return rawMessage.substr(0, firstSpace);
}

std::string Parser::extractChannelNames(
	const std::string& rawMessage,
	StringSizeT commandPrefixLength,
	StringSizeT keyStart
) {
	return (keyStart != std::string::npos)
		? rawMessage.substr(commandPrefixLength, keyStart - commandPrefixLength)
		: rawMessage.substr(commandPrefixLength);
}

std::string Parser::extractChannelKeys(const std::string& rawMessage, StringSizeT keyStart) {
	if (keyStart == std::string::npos) {
		return "";
	}

	StringSizeT keyStartTrimmed = rawMessage.find_first_not_of(' ', keyStart);
	if (keyStartTrimmed == std::string::npos) {
		return "";
	}

	StringSizeT keyEnd = rawMessage.find(' ', keyStartTrimmed);
	return (keyEnd != std::string::npos)
		? rawMessage.substr(keyStartTrimmed, keyEnd - keyStartTrimmed)
		: rawMessage.substr(keyStartTrimmed);
}

CommandType Parser::getCommandType(const std::string& command) {
	static CommandMap commands;
	if (commands.empty()) {
		commands["PASS"] = CMD_PASS;
		commands["NICK"] = CMD_NICK;
		commands["USER"] = CMD_USER;
		commands["JOIN"] = CMD_JOIN;
		commands["PRIVMSG"] = CMD_PRIVMSG;
		commands["KICK"] = CMD_KICK;
		commands["INVITE"] = CMD_INVITE;
		commands["TOPIC"] = CMD_TOPIC;
		commands["MODE"] = CMD_MODE;
		commands["PART"] = CMD_PART;
		commands["QUIT"] = CMD_QUIT;
	}

	CommandMapConstIterator it = commands.find(command);
	CommandType cmd = it != commands.end() ? it->second : CMD_UNKNOWN;

	return cmd;
}

/*
/	Channel names are strings (beginning with specified prefix characters).
/	First character must be a valid channel type prefix character ('#', '&').
/	They MUST NOT contain the following characters:
/	space (' ', 0x20),
/	control G / BELL ('^G', 0x07),
/	comma (',', 0x2C) (which is used as a list item separator by the protocol).
*/
bool Parser::validateChannelName(const std::string& channelName) {
	if (channelName.empty()) {
		return false;
	}
	if (!isValidChannelPrefix(*channelName.begin())) {
		return false;
	}
	if (containsChannelForbiddenChars(channelName)) {
		return false;
	}
	return true;
}

bool Parser::isValidChannelPrefix(char c) {
	const std::string valid = "#&";
	return valid.find(c) != std::string::npos;
}

bool Parser::isChannelForbiddenChar(char c) {
	const std::string forbidden = " ^G,";
	return forbidden.find(c) != std::string::npos;
}

bool Parser::containsChannelForbiddenChars(const std::string& input) {
	return std::find_if(
			input.begin(),
			input.end(),
			isChannelForbiddenChar
		) != input.end();
}

/*
/ Nicknames are non-empty strings with the following restrictions:
/ They MUST NOT contain any of the following characters:
/	space (' ', 0x20),
/	comma (',', 0x2C),
/	asterisk ('*', 0x2A),
/	question mark ('?', 0x3F),
/	exclamation mark ('!', 0x21),
/	at sign ('@', 0x40).
/ They MUST NOT start with any of the following characters:
/	dollar ('$', 0x24),
/	colon (':', 0x3A).
/ They MUST NOT start with a character listed as a:
/	channel type ('#', '&', '~', '%', '+'),
/	channel membership prefix (already listed above),
/	or prefix listed in the IRCv3 multi-prefix Extension (already listed above).
/ They SHOULD NOT contain any dot character ('.', 0x2E).
*/
bool Parser::validateNickname(const std::string& nickname) {
	if (nickname.empty()) {
		return false;
	}
	if (isNicknameForbiddenFirstChar(*nickname.begin())) {
		return false;
	}
	if (containsNicknameForbiddenChars(nickname)) {
		return false;
	}
	return true;
}

bool Parser::isNicknameForbiddenChar(char c) {
	const std::string forbidden = " ,*?!@.";
	return forbidden.find(c) != std::string::npos;
}

bool Parser::isNicknameForbiddenFirstChar(char c) {
	const std::string forbidden = "$:#&~%+";
	return forbidden.find(c) != std::string::npos;
}

bool Parser::containsNicknameForbiddenChars(const std::string& input) {
	return std::find_if(
			input.begin(),
			input.end(),
			isNicknameForbiddenChar
		) != input.end();
}

StringMap Parser::mapChanneslWithKeys(
	const std::string& channelNames,
	const std::string& channelKeys
) {
	StringMap channelKeyMap;

	std::list<std::string> channels = splitStringToList(channelNames, ",");
	std::list<std::string> keys;
	if (!channelKeys.empty()) {
		keys = splitStringToList(channelKeys, ",");
	}

	std::list<std::string>::iterator chanIt = channels.begin();
	std::list<std::string>::iterator keyIt = keys.begin();

	for ( ; chanIt != channels.end(); ++chanIt) {
		std::string key;
		if (keyIt != keys.end()) {
			key = *keyIt;
			++keyIt;
		} else {
			key = "";
		}
		channelKeyMap[*chanIt] = key;
	}

	return channelKeyMap;
}

std::list<std::string> Parser::splitStringToList(
	const std::string& values,
	const std::string& delimiter
) {
	std::list<std::string> result;
	StringSizeT start = 0;
	StringSizeT end;
	StringSizeT delimiterLength = 1;

	while ((end = values.find(delimiter, start)) != std::string::npos) {
		if (end > start) {
			result.push_back(values.substr(start, end - start));
		}
		start = end + delimiterLength;
	}

	if (start < values.length()) {
		result.push_back(values.substr(start));
	}

	return result;
}

void Parser::ft_error(const std::string& errorMessage) {
	std::cerr << "Error: " << errorMessage << std::endl;
}

std::string Parser::getTimestamp() {
	std::time_t now = std::time(NULL);
	std::ostringstream oss;
	oss << now;
	std::string setAtString = oss.str();

	return setAtString;
}

std::string Parser::numericReplyToString(NumericReply numericCode) {
	std::ostringstream oss;
	oss << static_cast<int>(numericCode);
	std::string code = oss.str();

	return code;
}

void Parser::debugPrintUsers(const std::vector<User*>& users) {
	std::cout << "[Debug] Users (" << users.size() << "):" << std::endl;
	for (std::size_t i = 0; i < users.size(); ++i) {
		if (users[i]) {
			std::cout << "  [" << i << "] "
					<< users[i]->getUserIdentifier()
					<< " @ " << users[i] << std::endl;
		} else {
			std::cout << "  [" << i << "] (null)" << std::endl;
		}
	}
}

void Parser::debugPrintChannels(const std::vector<Channel*>& channels) {
	std::cout << "[Debug] Channels (" << channels.size() << "):" << std::endl;
	for (std::size_t i = 0; i < channels.size(); ++i) {
		if (channels[i]) {
			std::cout << "  [" << i << "] "
					<< channels[i]->getName()
					<< " @ " << channels[i] << std::endl;
		} else {
			std::cout << "  [" << i << "] (null)" << std::endl;
		}
	}
}
