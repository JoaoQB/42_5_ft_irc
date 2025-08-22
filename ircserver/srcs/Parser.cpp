/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpetrukh <dpetrukh@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:59:51 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/08/22 16:01:03 by dpetrukh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

Parser::Parser() {
}

std::string Parser::extractCommand(const std::string& rawMessage) {
	std::string::size_type firstSpace = rawMessage.find(' ');
	if (firstSpace == std::string::npos) {
		return rawMessage;
	}
	return rawMessage.substr(0, firstSpace);
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

std::string Parser::extractParams(const std::string rawMessage, const std::string cmd) {
	size_t pos = rawMessage.find(cmd);

	if (pos == std::string::npos)
		return "";

	pos += cmd.length();

	if (pos < rawMessage.size() && std::isspace(rawMessage[pos]))
		pos++;

	return rawMessage.substr(pos);
}

std::string Parser::extractFirstParam(const std::string parameters) {
	std::size_t pos = parameters.find(' ');

	if (pos != std::string::npos)
		return parameters.substr(0, pos);

	return (parameters);
}

// Remove \r\n from string end
std::string Parser::trimCRLF(const std::string &s) {
	size_t end = s.size();
	if (!s.empty() && s[end-1] == '\n')
		end--;
	if (!s.empty() && s[end-1] == '\r')
		end--;

	return s.substr(0, end);
}

bool Parser::isNicknameForbiddenChar(char c) {
	const std::string forbidden = " ,*?!@.\n\r";
	return forbidden.find(c) != std::string::npos;
}

bool Parser::isNicknameForbiddenFirstChar(char c) {
	const std::string forbidden = "0123456789$:#&~%+";
	return forbidden.find(c) != std::string::npos;
}

bool Parser::containsNicknameForbiddenChars(const std::string& input) {
	return std::find_if(
			input.begin(),
			input.end(),
			isNicknameForbiddenChar
		) != input.end();
}

// Se nickname "PASS" || " PASS" || "PASS "
// Return true
// Se nickname "PASSagem"
// Return false
bool Parser::nicknameIsCommand(const std::string& nickname) {
	std::string commands[] = {"PASS", "NICK", "USER", "JOIN", "PRIVMSG", "KICK", "INVITE", "TOPIC", "MODE", "PART", "QUIT"};
	const int numCommands = sizeof(commands) / sizeof(commands[0]);

	for (int i = 0; i < numCommands; ++i) {
		std::size_t pos = nickname.find(commands[i]);
		if (pos != std::string::npos) {
			bool at_start = (pos == 0 || nickname[pos - 1] == ' ');
			bool at_end = (pos + commands[i].size() == nickname.size() || nickname[pos + commands[i].size()] == ' ');
			if (at_start || at_end)
				return true;
 		}
	}
	return false;
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
	if (nicknameIsCommand(nickname)) {
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

// TODO Confirm it's working
std::list<std::string> Parser::splitStringToList(
	const std::string& values,
	const std::string& delimiter
) {
	std::list<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type end;
	std::string::size_type delimiterLength = 1;

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

// TODO comfirm it's working
std::map<std::string, std::string> Parser::divideJoinCommand(
	const std::string& channelNames,
	const std::string& channelKeys
) {
	std::map<std::string, std::string> channelKeyMap;
	std::list<std::string> channels = splitStringToList(channelNames, ",");
	std::list<std::string> keys = splitStringToList(channelKeys, ",");

	return channelKeyMap;
}

