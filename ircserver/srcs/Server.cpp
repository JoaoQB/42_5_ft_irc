//
//
//

#include "../includes/Server.hpp"

bool Server::signal = false;

Server::Server()
	: name("ft_irc")
	, serverSocketFd(-1)
	, serverPort(-1)
	, serverPassword()
	, version("1.0")
	, users()
	, pollFds()
	, channels() {
	serverCreationTime = Parser::getTimestamp();
}

void Server::serverInit(const std::string& port, const std::string& password) {
	//TODO parse input
	const int serverPort = std::atoi(port.c_str());
	this->serverPort = serverPort;
	serverPassword = password;

	serverSocketCreate();

	std::cout << GREEN << "Server <" << serverSocketFd << "> Connected" << WHITE << std::endl;
	std::cout << "Waiting to accept a connection...\n" << RESET << std::endl;

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

void Server::signalHandler(int signum) {
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::signal = true;
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

void Server::closeFds() {
	// Close all Users
	for (UserListIterator it = users.begin() ; it != users.end() ; ++it) {
		std::cout << RED << "User <" << it->getFd() << "> Disconnected" << WHITE << RESET  << std::endl;
		close(it->getFd());
	}
	// Close server socket
	if (serverSocketFd != -1) {
		std::cout << RED << "Server <" << serverSocketFd << "> Disconnected" << WHITE << RESET << std::endl;
		close(serverSocketFd);
	}
}

//TODO remove user from channels
void Server::clearUserFromPoll(int fd) {
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

	std::cout << GREEN << "User <" << incomingFd << "> Connected" << WHITE << RESET << std::endl;
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
		std::cout << RED << "User <" << fd << "> Disconnected" << WHITE << RESET << std::endl;
		clearUserFromPoll(fd);
		close(fd);
		return;
	}

	buffer[bytes] = '\0';
	std::cout << YELLOW << "User <" << fd << "> Data: " << WHITE << buffer << RESET << std::endl;
	try {
		handleRawMessage(fd, buffer);
	} catch (const std::exception& e) {
		std::cerr << "handleRawMessage: " << e.what() << std::endl;
	}
}

//TODO Command Handlers
void Server::handleRawMessage(int fd, const char *buffer) {
	User &user = getUserByFd(fd);
	std::string rawMessage(buffer); // rawMessage = "PASS mypassword"
	std::string trimmedMessage = Parser::trimCRLF(rawMessage);
	std::string command = Parser::extractFirstParam(trimmedMessage); // command = "PASS"
	std::string params = Parser::extractFromSecondParam(trimmedMessage); // params = "mypassword"
	CommandType cmd = Parser::getCommandType(command); // cmd = CMD_PASS

	// Usuário está retrito a fazer outros comandos enquanto que não está autenticado no servidor
	if (!Parser::isAuthentication(user, cmd)) {
		std::cout << "ERR_NOTREGISTERED (451)" << std::endl;
		return;
	}

	switch (cmd) {
		case CAP:
			break;
		case CMD_PASS: {
			handlePassCommand(user, params);
			break;
		}
		case CMD_NICK: {
			handleNickCommand(user, params);
			break;
		}
		case CMD_USER:
			handleUserCommand(user, params);
			break;
		case CMD_JOIN:
			handleJoinCommand(user, params);
			break;
		case CMD_PRIVMSG:
			break;
		case CMD_KICK:
			break;
		case CMD_INVITE:
			break;
		case CMD_TOPIC:
			handleTopicCommand(user, params);
			break;
		case CMD_MODE:
			break;
		case CMD_PART:
			break;
		case CMD_QUIT:
			break;
		case CMD_PING:
			handlePingQuery(user, params);
			break;
		case CMD_WHO:
			handleWhoQuery(user, params);
			break;
		default:
			break;
	}
}

void Server::sendMessage(int userFd, const std::string &message) {
	if (userFd < 0)
	return;

	// IRC messages must end with CRLF
	std::string messageToSend = message + "\r\n";

	ssize_t bytesSent = send(userFd, messageToSend.c_str(), messageToSend.size(), 0);
	std::cout << "[DEBUG!] Sending:\n" << messageToSend << "To user: " << userFd << "!\n";
	if (bytesSent == -1) {
		std::cerr << "Failed to send message to fd " << userFd << std::endl;
		// TODO Handle Disconnect???
	}
}

void Server::sendNumericReply(
	const User* user,
	NumericReply numericCode,
	const std::string& message
) {
	if (!user) return;

	std::string reply = ":" + this->name
		+ " " + Parser::numericReplyToString(numericCode)
		+ " " + user->getNickname();

	if (!message.empty()) {
		reply += " " + message;
	}

	sendMessage(user->getFd(), reply);
}

void Server::debugPrintUsersAndChannels() const {
	std::cout << "\n" BOLD CYAN "==== Channels ====" RESET "\n";
	for (ChannelListConstIterator cIt = channels.begin(); cIt != channels.end(); ++cIt) {
		const Channel& ch = *cIt;
		std::cout << YELLOW "Channel: " << ch.getName() << RESET "\n"
				<< DIM " | Address: " << &ch << RESET "\n"
				<< " | Users: " << ch.getUsers().size()
				<< " | Operators: " << ch.getOperators().size() << "\n";

		// Print users in channel
		std::cout << "  " BOLD GREEN "Users:" RESET "\n";
		const std::vector<User*>& usersInCh = ch.getUsers();
		for (size_t i = 0; i < usersInCh.size(); ++i) {
			const User* u = usersInCh[i];
			std::cout << "    [" << i << "] " GREEN << u->getUserIdentifier() << RESET
					<< DIM " | Addr: " << u << RESET "\n";
		}

		// Print operators
		std::cout << "  " BOLD MAGENTA "Operators:" RESET "\n";
		const std::vector<User*>& opsInCh = ch.getOperators();
		for (size_t i = 0; i < opsInCh.size(); ++i) {
			const User* u = opsInCh[i];
			std::cout << "    [" << i << "] " MAGENTA << u->getUserIdentifier() << RESET
					<< DIM " | Addr: " << u << RESET "\n";
		}

		// Print channel modes
		std::cout << "  " BOLD "Modes: " RESET;
		const std::vector<std::string>& modes = ch.getChannelModes();
		for (size_t i = 0; i < modes.size(); ++i) {
			std::cout << WHITE << modes[i] << " " RESET;
		}
		std::cout << "\n\n";
	}

	std::cout << BOLD CYAN "\n==== Users ====" RESET "\n";
	for (std::list<User>::const_iterator uIt = users.begin(); uIt != users.end(); ++uIt) {
		const User& user = *uIt;
		std::cout << GREEN "User: " << user.getUserIdentifier() << RESET "\n"
				<< DIM " | Addr: " << &user
				<< " | FD: " << user.getFd()
				<< " | Registered: " << (user.isRegistered() ? "yes" : "no") << RESET "\n";

		// Print channels for this user
		std::cout << "  " BOLD YELLOW "Channels:" RESET "\n";
		const std::vector<Channel*>& chans = user.getChannels();
		for (size_t i = 0; i < chans.size(); ++i) {
			const Channel* ch = chans[i];
			std::cout << "    [" << i << "] " YELLOW << ch->getName() << RESET
					<< DIM " | Addr: " << ch << RESET "\n";
		}
		std::cout << "\n";
	}
	std::cout << BOLD CYAN "===================" RESET "\n";
}
