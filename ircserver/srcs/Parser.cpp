//
//
//

#include "../includes/Parser.hpp"
#include "../includes/User.hpp"

Parser::Parser() {
}

std::string Parser::extractFirstParam(const std::string& parameters) {
	std::size_t start = parameters.find_first_not_of(' ');
	if (start == std::string::npos) {
		return "";
	}

	std::size_t end = parameters.find(' ', start);
	std::string firstParam = end == std::string::npos
		? parameters.substr(start)
		: parameters.substr(start, end - start);
	// std::cout << "[DEBUG] First Param: '" << firstParam << "'\n";

	return firstParam;
}

std::string Parser::extractSecondParam(const std::string& parameters) {
	std::size_t firstSpace = parameters.find(' ');

	if (firstSpace == std::string::npos) {
		return "";
	}

	StringSizeT secondParamStartTrimmed = parameters.find_first_not_of(' ', firstSpace);
	if (secondParamStartTrimmed == std::string::npos) {
		return "";
	}

	StringSizeT keyEnd = parameters.find(' ', secondParamStartTrimmed);
	std::string secondParam = (keyEnd != std::string::npos)
		? parameters.substr(secondParamStartTrimmed, keyEnd - secondParamStartTrimmed)
		: parameters.substr(secondParamStartTrimmed);

	// std::cout << "[DEBUG] Second Param: '" << secondParam << "'\n";
	return secondParam;
}

std::string Parser::extractFromSecondParam(const std::string& parameters) {
	std::size_t firstSpace = parameters.find(' ');

	if (firstSpace == std::string::npos) {
		return "";
	}

	StringSizeT secondParamStartTrimmed = parameters.find_first_not_of(' ', firstSpace);
	if (secondParamStartTrimmed == std::string::npos) {
		return "";
	}

	std::string result = parameters.substr(secondParamStartTrimmed);
	// std::cout << "[DEBUG] Result: '" << result << "'\n";

	return result;
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
		commands["CAP"] = CAP;
	}

	CommandMapConstIterator it = commands.find(command);
	CommandType cmd = it != commands.end() ? it->second : CMD_UNKNOWN;

	return cmd;
}

bool Parser::isAuthentication(const User& user, const CommandType& command) {
	if (command != CAP && command != CMD_PASS
		&& command != CMD_USER && command != CMD_NICK
		&& !user.isRegistered()) {
		return false;
	}
	return true;
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

// Remove \r\n from string end
std::string Parser::trimCRLF(const std::string &s) {
	size_t end = s.size();
	if (!s.empty() && s[end-1] == '\n')
		end--;
	if (!s.empty() && s[end-1] == '\r')
		end--;

	return s.substr(0, end);
}

std::string Parser::trimWhitespace(const std::string &string) {
	size_t end = string.size();
	while (end > 0 && std::isspace(static_cast<unsigned char>(string[end - 1]))) {
		--end;
	}
	return string.substr(0, end);
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

std::string Parser::formatTimeStamp(const std::string &timestampStr) {
	// Converte string para time_t
	time_t timestamp = static_cast<time_t>(std::atoi(timestampStr.c_str()));

	struct tm *tm_info = localtime(&timestamp);
	if (!tm_info)
		return "";

	char buffer[64];
	// Formato: DD Mon YYYY HH:MM:SS
	strftime(buffer, sizeof(buffer), "%d %b %Y %H:%M:%S", tm_info);

	return std::string(buffer);
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
