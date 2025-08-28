/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpetrukh <dpetrukh@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:54:17 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/08/22 15:43:06 by dpetrukh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "./Common.hpp"

class Parser {
	public:
		// Extractors
		static std::string extractCommand(const std::string& rawMessage);
		static std::string extractChannelNames(
			const std::string& rawMessage,
			StringSizeT keyStart
		);
		static std::string extractChannelKeys(const std::string& rawMessage, StringSizeT keyStart);

		// Command classification
		static CommandType getCommandType(const std::string& command);

		// Validation
		static bool validateChannelName(const std::string& channelName);
		static bool validateNickname(const std::string& nickname);

		// Mappers
		static StringMap mapChanneslWithKeys(
			const std::string& channelNames,
			const std::string& channelKeys
		);

		static void ft_error(const std::string& errorMessage);
		static std::string getTimestamp();
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

	public:

		static std::string extractCommand(const std::string& rawMessage);

		static CommandType getCommandType(const std::string& command);

		static std::string extractParams(const std::string rawMessage, const std::string cmd);

		static std::string extractFirstParam(const std::string parameters);

		static bool validateChannelName(const std::string& channelName);

		static bool validateNickname(const std::string& nickname);

		static std::string trimCRLF(const std::string &s);

		std::map<std::string, std::string> divideJoinCommand(
			const std::string& channelNames,
			const std::string& channelKeys
		);
};

#endif
