/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:54:17 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 09:39:28 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "./Common.hpp"

class Parser {
	private:
		Parser();
		static bool isNicknameForbiddenFirstChar(char c);
		static bool isNicknameForbiddenChar(char c);
		static bool containsNicknameForbiddenChars(const std::string& input);

		static bool isValidChannelPrefix(char c);
		static bool isChannelForbiddenChar(char c);
		static bool containsChannelForbiddenChars(const std::string& input);

		static std::list<std::string> splitStringToList(
			const std::string& values,
			const std::string& delimiter
		);

	public:

		static std::string extractCommand(const std::string& rawMessage);

		static CommandType getCommandType(const std::string& command);

		static bool validateChannelName(const std::string& channelName);

		static bool validateNickname(const std::string& nickname);

		static StringMap mapJoinCommand(
			const std::string& channelNames,
			const std::string& channelKeys
		);

		// TODO Add error number and accurate message
		static void ft_error(const std::string& errorMessage);
};

#endif
