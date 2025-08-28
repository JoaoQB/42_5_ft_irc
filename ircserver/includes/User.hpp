/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpetrukh <dpetrukh@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:21 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/08/22 17:02:35 by dpetrukh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include "./Common.hpp"
#include "Channel.hpp"

class User {
	public:
		User();

		int getFd() const;
		const std::string& getRealName() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getPassword() const;
		std::string getUserIdentifier() const;
		bool isRegistered(void);
		std::vector<Channel*>& getChannels();

		void setFd(int fd);
		void setIpAddress(const std::string& ipAddr);
		void setRealName(std::string realname);
		void setUserName(std::string username);
		void setNickName(std::string nickname);
		void setPassword(std::string password);
		void setRegistered(bool registered);

		bool hasChannel(const Channel* channel) const;
		void addChannel(Channel* channel);
		void removeChannel(Channel* channel);


	private:
		int fd;
		std::string ipAddress;
		std::string realname;
		std::string username;
		std::string nickname;
		std::string password;
		bool registered;

		std::vector<Channel*> userChannels;

		const std::string& getIpAddress() const;
};

#endif
