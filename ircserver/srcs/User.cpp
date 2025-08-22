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

User::User() {
}

int User::getFd() {
	return this->fd;
}

void User::setFd(int fd) {
	this->fd = fd;
}

void User::setIpAddress(std::string ipAddress) {
	this->ipAddress = ipAddress;
}

std::string User::getRealName(void) {
	return this->realname;
}

void User::setRealName(std::string realname) {
	this->realname = realname;
}

std::string User::getUserName(void) {
	return this->username;
}

void User::setUserName(std::string username) {
	this->username = username;
}

std::string User::getNickName(void) {
	return this->nickname;
}

void User::setNickName(std::string nickname) {
	this->nickname = nickname;
}

std::string User::getPassword(void) {
	return this->password;
}

void User::setPassword(std::string password) {
	this->password = password;
}
