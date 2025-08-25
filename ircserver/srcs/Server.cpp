/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:14 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:18:14 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::signal = false;

Server::Server()
	: serverSocketFd(-1)
	, serverPort(-1)
	, serverPassword()
	, users()
	, pollFds()
	, channels() {
}

void Server::serverInit(const std::string& port, const std::string& password) {
	//TODO parse input
	const int serverPort = std::atoi(port.c_str());
	this->serverPort = serverPort;
	serverPassword = password;

	serverSocketCreate();

	std::cout << GRE << "Server <" << serverSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	// Run Server until a signal is received
	while (Server::signal == false) {
		// Wait for an event
		if ((poll(&pollFds[0], pollFds.size(), -1) == -1) &&
			Server::signal == false) {
			throw std::runtime_error("poll() failed");
		}

		// Use index loop instead of iterators, because of erase/push_back
		for (size_t i = 0 ; i < pollFds.size() ; ++i) {
			// Check if there's data to read
			if (pollFds[i].revents & POLLIN) {
				if (pollFds[i].fd == serverSocketFd) {
					acceptNewUser();
				} else {
					receiveNewData(pollFds[i].fd);
				}
			}
		}
	}
	closeFds();
}

void Server::serverSocketCreate() {
	struct sockaddr_in address;
	struct pollfd newPoll;

	// Set the address family to ipv4
	address.sin_family = AF_INET;
	// Convert the port to network byte order (big endian)
	address.sin_port = htons(serverPort);
	// Set the address to any local machine address
	address.sin_addr.s_addr = INADDR_ANY;

	// Create the server socket
	serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketFd == -1) {
		throw std::runtime_error("failed to create socket");
	}

	int en = 1;
	// Set the socket option (SO_REUSEADDR) to reuse the address
	if (setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) {
		throw std::runtime_error("failed to set option <SO_REUSEADDR> on socket");
	}
	// Set the socket option (O_NONBLOCK) for non-blocking socket
	if (fcntl(serverSocketFd, F_SETFL, O_NONBLOCK) == -1) {
		throw std::runtime_error("failed to set option <SO_NONBLOCK> on socket");
	}
	// Bind the socket to the address
	if (bind(serverSocketFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
		throw std::runtime_error("failed to bind socket");
	}
	// Listen for incoming connections and make the socket a passive socket
	if (listen(serverSocketFd, SOMAXCONN) == -1) {
		throw std::runtime_error("listen() failed");
	}

	// Add the server socket to the pollfd
	newPoll.fd = serverSocketFd;
	// Set the event to POLLIN for reading data
	newPoll.events = POLLIN;
	// Set the revents to 0
	newPoll.revents = 0;
	// Add the server socket to the pollFds container
	pollFds.push_back(newPoll);
}

void Server::signalHandler(int signum) {
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::signal = true;
}

void Server::closeFds() {
	// Close all Users
	for (UserListIterator it = users.begin() ; it != users.end() ; ++it) {
		std::cout << RED << "User <" << it->getFd() << "> Disconnected" << WHI << "\n";
		close(it->getFd());
	}
	// Close server socket
	if (serverSocketFd != -1) {
		std::cout << RED << "Server <" << serverSocketFd << "> Disconnected" << WHI << std::endl;
		close(serverSocketFd);
	}
}

//TODO remove user from channels
void Server::clearUser(int fd) {
	for (PollIterator pIt = pollFds.begin() ; pIt != pollFds.end() ; ++pIt) {
		if (pIt->fd == fd) {
			pollFds.erase(pIt);
			break ;
		}
	}
	for (UserListIterator cIt = users.begin() ; cIt != users.end() ; ++cIt) {
		if (cIt->getFd() == fd) {
			users.erase(cIt);
			break;
		}
	}
	//TODO check if user is last from channel and if so clear channel
}

void Server::acceptNewUser() {
	User	newUser;
	struct sockaddr_in UserAddress;
	struct pollfd newPoll;
	socklen_t len = sizeof(UserAddress);

	int incomingFd = accept(serverSocketFd, reinterpret_cast<sockaddr*>(&UserAddress), &len);
	if (incomingFd == -1) {
		std::cerr << "accept() failed" << std::endl;
		return;
	}
	// Set the socket option (O_NONBLOCK) for non-blocking socket
	if (fcntl(incomingFd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl() failed" << std::endl;
		return;
	}

	newPoll.fd = incomingFd;
	newPoll.events = POLLIN;
	newPoll.revents = 0;

	newUser.setFd(incomingFd);
	// Convert the ip address to string and set it
	newUser.setIpAddress(inet_ntoa(UserAddress.sin_addr));
	users.push_back(newUser);
	pollFds.push_back(newPoll);

	std::cout << GRE << "User <" << incomingFd << "> Connected" << WHI << std::endl;
}

void Server::receiveNewData(int fd) {
	// Buffer for incoming data
	char buffer[BUFFER_SIZE];
	// Clear buffer
	memset(buffer, 0, sizeof(buffer));

	// Read N bytes into BUF from socket FD, i.e. receive data
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	// Check if User is disconnected
	if (bytes <= 0) {
		std::cout << RED << "User <" << fd << "> Disconnected" << WHI << std::endl;
		clearUser(fd);
		close(fd);
		return;
	}

	buffer[bytes] = '\0';
	std::cout << YEL << "User <" << fd << "> Data: " << WHI << buffer;
	handleRawMessage(fd, buffer);
}

//TODO Command Handlers
void Server::handleRawMessage(int fd, const char *buffer) {
	// Remove line break char from end of buffer
	std::string rawMessage(buffer);
	StringSizeT lineBreak = 1;
	StringSizeT trimmedLength = rawMessage.length() - lineBreak;
	std::string trimmedMessage = rawMessage.substr(0, trimmedLength);

	std::string command = Parser::extractCommand(trimmedMessage);
	CommandType cmd = Parser::getCommandType(command);

	switch (cmd) {
		case CMD_PASS:
			break;
		case CMD_NICK:
			break;
		case CMD_USER:
			break;
		case CMD_JOIN:
			handleJoinCommand(fd, trimmedMessage);
			break;
		case CMD_PRIVMSG:
			break;
		case CMD_KICK:
			break;
		case CMD_INVITE:
			break;
		case CMD_TOPIC:
			break;
		case CMD_MODE:
			break;
		case CMD_PART:
			break;
		case CMD_QUIT:
			break;
		default:
			break;
	}
}

void Server::handleJoinCommand(int fd, const std::string& rawMessage) {
	const StringSizeT commandPrefixLength = 5; // Account for space after 'JOIN'
	if (rawMessage.length() <= commandPrefixLength) {
		Parser::ft_error("empty JOIN command");
		return;
	}

	//TODO create helper functions for this logic.
	StringSizeT keyStart = rawMessage.find(' ', commandPrefixLength);
	std::string channelNames = (keyStart != std::string::npos)
		? rawMessage.substr(commandPrefixLength, keyStart - commandPrefixLength)
		: rawMessage.substr(commandPrefixLength);

	std::string channelKeys;
	if (keyStart != std::string::npos) {
		StringSizeT keyStartTrimmed = rawMessage.find_first_not_of(' ', keyStart);
		if (keyStartTrimmed != std::string::npos) {
			StringSizeT keyEnd = rawMessage.find(' ', keyStartTrimmed);
			channelKeys = (keyEnd != std::string::npos)
				? rawMessage.substr(keyStartTrimmed, keyEnd - keyStartTrimmed)
				: rawMessage.substr(keyStartTrimmed);
		}
	}

	StringMap channelsWithKeys = Parser::mapJoinCommand(channelNames, channelKeys);
	for (StringMapConstIterator it = channelsWithKeys.begin();
		it != channelsWithKeys.end();
		++it
	) {
		const std::string& name = it->first;
		const std::string& key = it->second;
		if (!Parser::validateChannelName(name)) {
			Parser::ft_error("invalid Channel name");
			continue;
		}
		if (channelExists(name)) {
			try {
				addUserToChannel(fd, name, key);
				// std::cout << "Added user " << fd << " to existing channel " << name << "\n";
			} catch (const std::exception& e) {
				std::cerr << "Failed to add user to channel: " << e.what() << std::endl;
			}
		}
		else {
			try {
				createChannel(fd, name, key);
			} catch (const std::exception& e) {
				std::cerr << "Failed to add user to channel: " << e.what() << std::endl;
			}
		}
	}
}

Channel& Server::getChannel(const std::string& channelName) {
	for (
		ChannelListIterator it = this->channels.begin() ;
		it != this->channels.end() ;
		++it
	) {
		if (it->getName() == channelName) {
			return *it;
		}
	}
	throw std::runtime_error("Channel not found");
}

bool Server::channelExists(const std::string& channelName) const {
	for (
		ChannelListConstIterator it = this->channels.begin() ;
		it != this->channels.end() ;
		++it
	) {
		if (it->getName() == channelName) {
			return true;
		}
	}
	return false;
}

void Server::createChannel(
	int userFd,
	const std::string& channelName,
	const std::string& channelKey
) {
	User& creator = getUser(userFd);
	Channel newChannel;
	newChannel.setName(channelName);
	if (!channelKey.empty()) {
		newChannel.setPassword(channelKey);
	}
	newChannel.addUser(&creator);
	newChannel.addOperator(&creator);
	this->channels.push_back(newChannel);
	Channel* joinedChannel = &this->channels.back();
	creator.addChannel(joinedChannel);
	std::cout << "Channel " << channelName << " created by user " << userFd << "\n";
}

void Server::addUserToChannel(
	int userFd,
	const std::string& channelName,
	const std::string& channelKey
) {
	Channel& targetChannel = getChannel(channelName);
	User& targetUser = getUser(userFd);
	if (targetChannel.hasUser(&targetUser)) {
		std::cout << "User " << userFd << " is already in channel " << channelName << std::endl;
		return;
	}
	if (targetChannel.isFull()) {
		Parser::ft_error("channel full");
		return ;
	}
	if (targetChannel.isInviteOnly()) {
		Parser::ft_error("channel is invite only");
		return ;
	}
	if (!targetChannel.requiresPassword() ||
		targetChannel.getPassword() == channelKey
	) {
		targetChannel.addUser(&targetUser);
		targetUser.addChannel(&targetChannel);
		std::cout << "User " << userFd << " added to channel " << channelName << std::endl;
	} else {
		Parser::ft_error("channel password is incorrect");
	}
}

User& Server::getUser(int fd) {
	for (
		UserListIterator it = this->users.begin() ;
		it != this->users.end() ;
		++it
	) {
		if (it->getFd() == fd) {
			return *it;
		}
	}
	throw std::runtime_error("User not found");
}
