/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpetrukh <dpetrukh@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:14 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/08/22 13:05:00 by dpetrukh         ###   ########.fr       */
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

void Server::acceptNewUser() {
	User	User;
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

	User.setFd(incomingFd);
	// Convert the ip address to string and set it
	User.setIpAddress(inet_ntoa(UserAddress.sin_addr));
	users.push_back(User);
	pollFds.push_back(newPoll);

	std::cout << GRE << "User <" << incomingFd << "> Connected" << WHI << std::endl;
}

void Server::receiveNewData(int fd) {
	// Buffer for incoming data
	char buffer[1024];
	// Clear buffer
	memset(buffer, 0, sizeof(buffer));

	// Read N bytes into BUF from socket FD, i.e. receive data
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	// Check if User is disconnected
	if (bytes <= 0) {
		std::cout << RED << "User <" << fd << "> Disconnected" << WHI << std::endl;
		clearUsers(fd);
		close(fd);
	} else {
		buffer[bytes] = '\0';
		std::cout << YEL << "User <" << fd << "> Data: " << WHI << buffer;
		handleRawMessage(buffer, fd);
		// TODO! Add code to process the received data:
		// parse, check, authenticate, handle the command, etc...
	}

}

void Server::signalHandler(int signum) {
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::signal = true;
}

void Server::closeFds() {
	// Close all Users
	for (UserIterator it = users.begin() ; it != users.end() ; ++it) {
		std::cout << RED << "User <" << it->getFd() << "> Disconnected" << WHI << "\n";
		close(it->getFd());
	}
	// Close server socket
	if (serverSocketFd != -1) {
		std::cout << RED << "Server <" << serverSocketFd << "> Disconnected" << WHI << std::endl;
		close(serverSocketFd);
	}
}

void Server::clearUsers(int fd) {
	for (pollIterator pIt = pollFds.begin() ; pIt != pollFds.end() ; ++pIt) {
		if (pIt->fd == fd) {
			pollFds.erase(pIt);
			break ;
		}
	}
	for (UserIterator cIt = users.begin() ; cIt != users.end() ; ++cIt) {
		if (cIt->getFd() == fd) {
			users.erase(cIt);
			break;
		}
	}
}

User* Server::getUserByFd(int fd){
	for (size_t i = 0; i < users.size(); i++) {
		if (users[i].getFd() == fd)
			return &users[i];
	}
	return NULL;
}

//TODO Command Handlers
void Server::handleRawMessage(const char *buffer, int fd) {
	std::string rawMessage(buffer);

	//rawMessage = "PASS mypassword"
	std::string command = Parser::extractCommand(rawMessage);
	// command = "PASS"
	CommandType cmd = Parser::getCommandType(command);
	// cmd = 1
	std::string params = Parser::extractParams(rawMessage, command);

	User *user = getUserByFd(fd);
	if (!user) {
		throw std::runtime_error("User not found for fd");
		return;
	}

	switch (cmd) {
		case CMD_PASS: {
			cmdPass(*user, params);
			break;
		}
		case CMD_NICK:
			break;
		case CMD_USER:
			break;
		case CMD_JOIN:
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

void Server::cmdPass(User &user, std::string cmdParameters){

	std::cout << "PASS parameters: " << cmdParameters << std::endl;

	if (cmdParameters.empty()) {
		std::cout << "Erro 461 needmoreparams" << std::endl;
	}

	// 1 - Se parametros vaziu ou Se Nick ou User já tiverem algo, erro
	if (!user.getNickName().empty() || !user.getUserName().empty()) {
		//sendError(user.getFd(), "462", "You may not reregister");
		std::cout << "Erro 462 alreadyregistered" << std::endl;
		return;
	}

	// 2 - Se comeca com ":", remover o ":" e aceitar espaços

	std::string password;

	if (!cmdParameters.empty() && cmdParameters[0] == ':')
		password = cmdParameters.substr(1);
	else { // Se NÃO ":", substr até primeiro espaço ou fim.
		size_t spacePos = cmdParameters.find(' ');
		if (spacePos != std::string::npos)
			password = cmdParameters.substr(0, spacePos);
		else
			password = cmdParameters;
	}

	// 3 - Remover o \r e \n no fim
	password = Parser::trimCRLF(password);

	// 4 - Verificar se a password coicide com o servidor, se errada 464 ERR_PASSWDMISMATCH.
	std::cout << "Password result: " << password << " length: " << password.length() << std::endl;
	std::cout << "Password server: " << this->serverPassword << " length: " << this->serverPassword.length() << std::endl;
	if (password != this->serverPassword) {
		std::cout << "Erro 464 passmismatch" << std::endl;
		return ;
	}

	// 5 - Adicionar ao user.password
	user.setPassword(password);
	std::cout << "Password Registered Successfully: " << user.getPassword() << std::endl;
}