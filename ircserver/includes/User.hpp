/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:21 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/10 09:39:25 by jqueijo-         ###   ########.fr       */
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
	private:
		int fd;
		std::string ipAddress;
		std::string realname;
		std::string username;
		std::string nickname;
		std::string password;
};

#endif
