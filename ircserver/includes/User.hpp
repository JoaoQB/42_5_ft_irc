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
		const std::string& getNickname() const;
		std::string getUserIdentifier() const;
		std::vector<Channel*>& getChannels();

		void setFd(int fd);
		void setIpAddress(const std::string& ipAddr);

		bool hasChannel(const Channel* channel) const;
		void addChannel(Channel* channel);
		void removeChannel(Channel* channel);


		std::string getRealName(void);
		void setRealName(std::string realname);

		std::string getUserName(void);
		void setUserName(std::string username);

		std::string getNickName(void);
		void setNickName(std::string nickname);

		std::string getPassword(void);
		void setPassword(std::string password);

		bool isRegistered(void);
		void setRegistered(bool registred);

	private:
		int fd;
		std::string ipAddress;
		std::string realname;
		std::string username;
		std::string nickname;
		std::string password;
		bool registered;

		std::vector<Channel*> userChannels;

		const std::string& getUsername() const;
		const std::string& getIpAddress() const;
};

#endif
