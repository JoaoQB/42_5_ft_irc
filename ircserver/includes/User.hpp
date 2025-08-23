/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:21 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:24:30 by jqueijo-         ###   ########.fr       */
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

		void setFd(int fd);
		void setIpAddress(const std::string& ipAddr);

		bool hasChannel(const Channel* channel) const;

		void addChannel(Channel* channel);

	private:
		int fd;
		std::string ipAddress;
		std::string realname;
		std::string username;
		std::string nickname;
		std::string password;

		std::vector<Channel*> userChannels;
};

#endif
