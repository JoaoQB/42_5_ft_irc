/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:54:17 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:28:20 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "./Common.hpp"

class Parser {
	public:
		// Extractors
		static std::string extractCommand(const std::string& rawMessage);
		std::string extractChannelNames(const std::string& rawJoinMessage);
		std::string extractChannelKeys(const std::string& rawJoinMessage);

		// Command classification
		static CommandType getCommandType(const std::string& command);

		// Validation
		static bool validateChannelName(const std::string& channelName);
		static bool validateNickname(const std::string& nickname);

		// Mappers
		static StringMap mapJoinCommand(
			const std::string& channelNames,
			const std::string& channelKeys
		);

		// TODO Add error number and accurate message
		static void ft_error(const std::string& errorMessage);

	private:
		Parser();

		// Nickname validation helpers
		static bool isNicknameForbiddenFirstChar(char c);
		static bool isNicknameForbiddenChar(char c);
		static bool containsNicknameForbiddenChars(const std::string& input);

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
