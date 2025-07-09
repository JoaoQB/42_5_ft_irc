/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:59:51 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/09 10:34:19 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

Parser::Parser() {
}

bool Parser::isNicknameForbiddenChar(char c) const {
	const std::string forbidden = " ,*?!@.";
	return forbidden.find(c) != std::string::npos;
}

bool Parser::isNicknameForbiddenFirstChar(char c) const {
	const std::string forbidden = "$:#&~%+";
	return forbidden.find(c) != std::string::npos;
}

bool Parser::containsNicknameForbiddenChars(const std::string& input) const {
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
bool Parser::parseNickname(const std::string& nickname) const {
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
