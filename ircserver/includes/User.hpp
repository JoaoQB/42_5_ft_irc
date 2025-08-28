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

class User {
	public:
		User();

		int getFd();

		void setFd(int fd);
		void setIpAddress(std::string ipAddress);

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
};

#endif
