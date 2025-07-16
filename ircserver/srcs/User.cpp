/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:16 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/06/27 12:48:00 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/User.hpp"

User::User()
	: fd(-1)
	, ipAddress()
	, realname()
	, username()
	, nickname()
	, password() {
}

int User::getFd() const {
	return this->fd;
}

void User::setFd(int fd) {
	this->fd = fd;
}

void User::setIpAddress(std::string ipAddress) {
	this->ipAddress = ipAddress;
}
