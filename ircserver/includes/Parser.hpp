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
		static StringMap mapModesWithParams(
			const StringVector& modeString,
			const std::string& modeParams
		);

		// Raw Message helpers
		static bool isSingleFullCommand(const std::string& receivedMessage);
		static std::string trimCRLF(const std::string &string);

		static std::string trimWhitespace(const std::string &string);
		static void ft_error(const std::string& errorMessage);
		static std::string getTimestamp();
		static std::string formatTimeStamp(const std::string &timestampStr);
		static std::string numericReplyToString(NumericReply numericCode);
		static bool isValidChannelPrefix(const char c);

		// Channel Modes Utilities
		static bool isValidChannelMode(const char mode);
		static bool isTypeAMode(const std::string& modeParameter);
		static bool isValidModeSign(const char sign);
		static bool modeNeedsParam(const std::string& mode);

		// Utility helpers
		static std::list<std::string> splitStringToList(
			const std::string& values,
			const std::string& delimiter
		);
		static std::set<std::string> splitStringToSet(const std::string& targets);
		static bool stringToInt(const std::string& str, int& result);

		// DEBUG
		static void debugPrintVector(const StringVector& vector);
		static void debugPrintUsers(const std::vector<User*>& users);
		static void debugPrintChannels(const std::vector<Channel*>& channels);

	private:
		Parser();

		// Nickname validation helpers
		static bool isNicknameForbiddenFirstChar(const char c);
		static bool isNicknameForbiddenChar(const char c);
		static bool containsNicknameForbiddenChars(const std::string& input);
		static bool nicknameIsCommand(const std::string& input);

		// Channel name validation helpers
		static bool isChannelForbiddenChar(const char c);
		static bool containsChannelForbiddenChars(const std::string& input);
};

#endif
