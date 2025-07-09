/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 09:54:17 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/09 10:32:05 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <algorithm>

class Parser {
	private:
		bool isNicknameForbiddenFirstChar(char c) const;
		bool isNicknameForbiddenChar(char c) const;
		bool containsNicknameForbiddenChars(const std::string& input) const;
	public:
		Parser();
		bool parseNickname(const std::string& nickname) const;
};

#endif
