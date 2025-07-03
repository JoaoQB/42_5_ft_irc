/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:21 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/03 15:47:15 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>

class Client {
	public:
		Client();

		int getFd();

		void setFd(int fd);
		void setIpAddress(std::string ipAddress);
	private:
		int fd;
		std::string ipAddress;
};

#endif
