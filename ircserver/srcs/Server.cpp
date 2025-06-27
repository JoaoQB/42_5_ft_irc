/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:14 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/06/27 13:25:40 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::signal = false;

Server::Server() {
	this->serverSocketFd = -1;
}

void Server::serverInit() {
	this->serverPort = 4444;
	serverSocketCreate();

	std::cout << GRE << "Server <" << this->serverSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
}

void Server::serverSocketCreate() {
	struct sockaddr_in address;
	struct pollfd newPoll;

	// Set the address family to ipv4
	address.sin_family = AF_INET;
	// Convert the port to network byte order (big endian)
	address.sin_port = htons(this->serverPort);
	// Set the address to any local machine address
	address.sin_addr.s_addr = INADDR_ANY;

	// Create the server socket
	this->serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverSocketFd == -1) {
		throw std::runtime_error("failed to create socket");
	}
	int en = 1;
}

void Server::signalHandler(int signum) {
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::signal = true;
}

void Server::closeFds() {
	// Close all clients
	for (std::vector<Client>::iterator it = this->clients.begin() ; it != this->clients.end() ; ++it) {
		std::cout << RED << "Client <" << it->getFd() << "> Disconnected" << WHI << "\n";
		close(it->getFd());
	}
	// Close server socket
	if (this->serverSocketFd != -1) {
		std::cout << RED << "Server <" << serverSocketFd << "> Disconnected" << WHI << std::endl;
		close(serverSocketFd);
	}
}

void Server::clearClients(int fd) {
	for (std::vector<struct pollfd>::iterator it = this->pollFds.begin() ; it != this->pollFds.end() ; ++it) {
		if (it->fd == fd) {
			pollFds.erase(it);
			break ;
		}
	}
	for (std::vector<Client>::iterator it = this->clients.begin() ; it != this->clients.end() ; ++it) {
		if (it->getFd() == fd) {
			clients.erase(it);
			break;
		}
	}
}
