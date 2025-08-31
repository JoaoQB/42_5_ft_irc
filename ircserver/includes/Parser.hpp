//
//
//

#ifndef PARSER_HPP
# define PARSER_HPP

# include "./Common.hpp"

class Parser {
	public:
		// Extractors
		static std::string extractFirstParam(const std::string& parameters);
		static std::string extractSecondParam(const std::string& parameters);
		static std::string extractFromSecondParam(const std::string& parameters);

		// Command classification
		static CommandType getCommandType(const std::string& command);
		static bool isAuthentication(const User& user, const CommandType& command);

		// Validation
		static bool validateChannelName(const std::string& channelName);
		static bool validateNickname(const std::string& nickname);

		// Mappers
		static StringMap mapChanneslWithKeys(
			const std::string& channelNames,
			const std::string& channelKeys
		);

		static std::string trimCRLF(const std::string &s);
		static std::string trimWhitespace(const std::string &string);
		static void ft_error(const std::string& errorMessage);
		static std::string getTimestamp();
		static std::string formatTimeStamp(const std::string &timestampStr);
		static std::string numericReplyToString(NumericReply numericCode);

		// DEBUG
		static void debugPrintUsers(const std::vector<User*>& users);
		static void debugPrintChannels(const std::vector<Channel*>& channels);

	private:
		Parser();

		// Nickname validation helpers
		static bool isNicknameForbiddenFirstChar(char c);
		static bool isNicknameForbiddenChar(char c);
		static bool containsNicknameForbiddenChars(const std::string& input);
		static bool nicknameIsCommand(const std::string& input);

		// Channel name validation helpers
		static bool isValidChannelPrefix(char c);
		static bool isChannelForbiddenChar(char c);
		static bool containsChannelForbiddenChars(const std::string& input);

		// Utility helpers
		static std::list<std::string> splitStringToList(
			const std::string& values,
			const std::string& delimiter
		);
};

#endif
