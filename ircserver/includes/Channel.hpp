/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 10:46:42 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/09 10:52:40 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "./User.hpp"
# include <string>
# include <vector>

class Channel {
	private:
		std::string name;
		std::string password;
		std::vector<User> channelUsers;

		Channel();

	public:
		Channel(const std::string& channelName);
		Channel(const std::string& channelName, const std::string& key);
};

#endif
