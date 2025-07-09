/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:54:17 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/09 14:39:54 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include "./Common.hpp"

class Parser {
	private:
		static bool isNicknameForbiddenFirstChar(char c);
		static bool isNicknameForbiddenChar(char c);
		static bool containsNicknameForbiddenChars(const std::string& input);

		static bool isValidChannelPrefix(char c);
		static bool isChannelForbiddenChar(char c);
		static bool containsChannelForbiddenChars(const std::string& input);

		std::list<std::string> splitStringToList(
			const std::string& values,
			const std::string& delimiter
		);

	public:
		Parser();

		bool validateNickname(const std::string& nickname) const;

		bool validateChannelName(const std::string& channelName) const;

		std::map<std::string, std::string> divideJoinCommand(
			const std::string& channelNames,
			const std::string& channelKeys
		);
};

#endif
