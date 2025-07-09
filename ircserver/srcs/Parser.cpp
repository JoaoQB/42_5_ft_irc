/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:59:51 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/09 14:41:01 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

Parser::Parser() {
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
bool Parser::validateNickname(const std::string& nickname) const {
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
bool Parser::validateChannelName(const std::string& channelName) const {
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
