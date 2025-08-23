/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 10:46:42 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:21:57 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "./Common.hpp"
# include "./User.hpp"

class Channel {
	public:
		Channel();

		// Accessors (getters)
		const std::string& getName() const;
		const std::string& getPassword() const;
		bool channelIsFull() const;
		bool requiresPassword() const;

		// Mutators (setters)
		void setName(const std::string& channelName);
		void setPassword(const std::string& key);

		// Actions
		void addUser(User* user);
		void addOperator(User* user);

		// Helpers
		bool hasUser(const User* user) const;

	private:
		std::string name;
		std::string password;
		std::string topic;

		bool hasPassword;
		bool isInviteOnly;
		bool isFull;

		int channelLimit;
		int usersInChannel;

		std::vector<User*> channelUsers;
		std::vector<User*> channelOperators;
};

#endif
