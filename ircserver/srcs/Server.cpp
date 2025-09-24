//
//
//

#include "../includes/Server.hpp"

bool Server::signal = false;

Server::Server()
	: name("irc:chat:42")
	, serverSocketFd(-1)
	, serverPort(-1)
	, serverPassword()
	, version("1.0")
	, users()
	, pollFds()
	, channels() {
	serverCreationTime = Parser::getTimestamp();
}

bool Server::parseInput(const std::string& port, const std::string& password) {
	if (port.empty() || password.empty()) {
		return false;
	}
	if (port.size() != 4 && port.size() != 5) {
		return false;
	}
	for (
		std::string::const_iterator it = port.begin();
		it != port.end();
		++it
	) {
		if (!std::isdigit(*it)) {
			return false;
		}
	}
	const int serverPort = std::atoi(port.c_str());
	if (serverPort < 1024 || serverPort > 65535) {
		return false;
	}
	if (password.size() > 25) {
		return false;
	}
	static const std::string whitespace = " \f\v\t\b\a\n\r";
	for (
		std::string::const_iterator it = password.begin();
		it != password.end();
		++it
	) {
		if (std::find(whitespace.begin(), whitespace.end(), *it) != whitespace.end()) {
			return false;
		}
	}
	return true;
}

void Server::serverInit(const std::string& port, const std::string& password) {
	if (!parseInput(port, password)) {
		std::cerr << "Error: invalid input, "
			<< "please provide a port between 1024 and 65535.\n"
			<< "Password max length = 25 chars and without whitespaces\n"
			<< "Usage: './ircserv <port> <password>'\n"
			<< "Example: './ircserv 6667 mypassword'" << std::endl;
		throw std::invalid_argument("Invalid input");
	}
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
		processPendingDisconnects();
	}
	handleServerShutdown();
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

void Server::handleServerShutdown() {
	const static std::string errorMessageBeguin = "ERROR :Closing Link: ";
	const static std::string errorMessageEnd = " (Server Shutdown)";
	for (
		UserListIterator it = users.begin();
		it != users.end();
		++it
	) {
		const std::string userInfo = it->getNickname() + "[" + it->getIpAddress() + "]";
		const std::string shutdownMessage = errorMessageBeguin
			+ userInfo + errorMessageEnd;
		sendMessage(it->getFd(), shutdownMessage);
	}
	closeFds();
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

	// Read N bytes into BUF from socket FD, i.e. receive data
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	// Check if User is disconnected
	if (bytes <= 0) {
		// No data available now, try later
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			std::cout << YELLOW << "User <" << fd << "> No data available, continuing" << WHITE << RESET << std::endl;
			return;
		}
		std::cout << RED << "User <" << fd << "> Disconnected" << WHITE << RESET << std::endl;
		disconnectUser(fd);
		return;
	}

	std::string receivedMessage(buffer, bytes);

	std::cout << YELLOW << "User <" << fd << "> Data: " << WHITE << receivedMessage << RESET << std::endl;


	// // DEBUG incoming chars
	// std::cout << "[DEBUG]\n";
	// buffer[bytes] = '\0';
	// for (size_t i = 0; buffer[i]; ++i) {
	// 	char c = buffer[i];
	// 	if (c == '\r') std::cout << "\\r";
	// 	else if (c == '\n') std::cout << "\\n";
	// 	else if (c < 32 || c > 126) std::cout << "\\x" << std::hex << (int)c;
	// 	else std::cout << c;
	// }
	// std::cout << std::endl;


	if (hasMessageInBuffer(fd) || !Parser::isSingleFullCommand(receivedMessage)) {
		processPendingMessages(fd, receivedMessage);
		return;
	}
	try {
		handleRawMessage(fd, receivedMessage);
	} catch (const std::exception& e) {
		std::cerr << "handleRawMessage: " << e.what() << std::endl;
	}
}

bool Server::hasMessageInBuffer(int targetFd) const {
	return rawMessageBuffers.find(targetFd) != rawMessageBuffers.end();
}

void Server::processPendingMessages(int fd, const std::string& receivedMessage) {
	// std::cout << "Buffer had:\n" << rawMessageBuffers[fd] << std::endl;
	rawMessageBuffers[fd].append(receivedMessage);
	StringSizeT newLineSize = 1;
	StringSizeT newLineIndex;
	while ((newLineIndex = rawMessageBuffers[fd].find("\n")) != std::string::npos) {
		std::string firstMessage = rawMessageBuffers[fd].substr(0, newLineIndex + newLineSize);
		rawMessageBuffers[fd].erase(0, newLineIndex + newLineSize);
		handleRawMessage(fd, firstMessage);
	}
	// std::cout << "Remaining buffer:\n" << rawMessageBuffers[fd] << std::endl;
}

void Server::handleRawMessage(int fd, const std::string& rawMessage) {
	User &user = getUserByFd(fd);
	std::string trimmedMessage = Parser::trimCRLF(rawMessage);
	// std::cout << "[DEBUG] handle raw message:\n" << rawMessage << std::endl;
	std::string command = Parser::extractFirstParam(trimmedMessage); // command = "PASS"
	CommandType cmd = Parser::getCommandType(command); // cmd = CMD_PASS
	std::string params = Parser::extractFromSecondParam(trimmedMessage); // params = "mypassword"

	// Usuário está retrito a fazer outros comandos enquanto que não está autenticado no servidor
	if (!Parser::isAuthentication(user, cmd)) {
		sendNumericReply(&user, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}

	switch (cmd) {
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
			handlePrivMsgCommand(user, params);
			break;
		case CMD_KICK:
			handleKickCommand(user, params);
			break;
		case CMD_INVITE:
			handleInviteCommand(user, params);
			break;
		case CMD_TOPIC:
			handleTopicCommand(user, params);
			break;
		case CMD_MODE:
			handleModeCommand(user, params);
			break;
		case CMD_PART:
			handlePartCommand(user, params);
			break;
		case CMD_QUIT:
			handleQuitCommand(user, params);
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

void Server::processPendingDisconnects() {
	std::vector<int> fdsToRemove;
	for (UserListIterator it = users.begin(); it != users.end(); ++it) {
		if (it->isPendingDisconnect()) {
			fdsToRemove.push_back(it->getFd());
		}
	}

	for (size_t i = 0; i < fdsToRemove.size(); ++i) {
		int fd = fdsToRemove[i];
		try {
			User& user = getUserByFd(fd);
			disconnectUserFromAllChannels(&user, true, ":Disconnected!");
		} catch (const std::exception &e) {
		}
		clearUser(fd);
		close(fd);
	}
	// debugPrintUsersAndChannels();
}

void Server::sendMessage(int userFd, const std::string &message) {
	if (userFd < 0 || !userExists(userFd)) {
		return;
	}

	try {
		User& user = getUserByFd(userFd);
		if (user.isPendingDisconnect()) {
			return;
		}

		// IRC messages must end with CRLF
		std::string messageToSend = message + "\r\n";
		ssize_t bytesSent = send(userFd, messageToSend.c_str(), messageToSend.size(), MSG_NOSIGNAL);

		// std::cout << "[DEBUG] Sending:\n" << messageToSend
		// 		<< "To user: " << userFd << "\n";

		if (bytesSent == -1) {
			// std::cerr << "SEND failed on fd " << userFd
			// 		<< ": " << strerror(errno) << "\n";
			user.setPendingDisconnect(true);
		}

	} catch (const std::exception& e) {
		std::cerr << "sendMessage exception: " << e.what() << std::endl;
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

void Server::processSingleTargetMessage(
	const User *senderUser,
	const std::string& target,
	const std::string& message
){
	// Se for um canal
	if (Parser::isValidChannelPrefix(*target.begin())) {
		try {
			Channel& channel = getChannel(*const_cast<User*>(senderUser), target);
			sendMessageToChannel(senderUser, channel, message);
		} catch (const std::runtime_error& e) {
			std::cerr << "PRIVMSG: " << e.what() << std::endl;
		}
		return;
	}
	// Se for um user
	sendMessageToUser(senderUser, target, message);
}

void Server::processSingleTargetKick(
	const User *user,
	Channel &targetChannel,
	const std::string &it,
	const std::string &reason
	) {

	// Procurar targetUser → se não existir → ERR_NOSUCHNICK (401)
	User& targetUser = Server::getUserByNickname(*user, it);

	std::string channelName = targetChannel.getName();

	// TargetUser está no canal? → se não → ERR_USERNOTINCHANNEL (441)
	if (!targetChannel.hasUser(&targetUser)) {
		sendNumericReply(user, ERR_USERNOTINCHANNEL ,
			targetUser.getNickname() +
			" " + channelName +
			" :You're not channel operator");
		return ;
	}

	const std::string username = targetUser.getNickname();
	const std::string broadcastMessage = channelName + " " + username + " " + reason;

	// Enviar broadcast → :<sender> KICK <channel> <target> :<reason> para todos no canal.
	Server::broadcastCommand(user->getUserIdentifier(), &targetChannel, "KICK", broadcastMessage);

	// Remover targetUser do canal (atualizar estrutura).
	targetChannel.removeUser(*this, &targetUser);

	// Se canal ficou vazio, opcionalmente apagar. !!!!CONFIRMAR COM O JOÃO!!!!
	if (targetChannel.isEmpty()) {
		this->removeChannel(&targetChannel);
		return ;
	}
}

void Server::sendMessageToChannel(
	const User* senderUser,
	const Channel& targetChannel,
	const std::string& message
) {
	// User NÃO está no channel que envia privmsg
	if (!targetChannel.hasUser(senderUser)) {
		sendNumericReply(senderUser, ERR_CANNOTSENDTOCHAN,
			targetChannel.getName() + " :Cannot send to channel");
		return ;
	}

	for (
		UserVectorConstIterator it = targetChannel.getUsers().begin();
		it != targetChannel.getUsers().end();
		++it
	) {
		if (*it != senderUser)
			sendMessageToUser(senderUser, (*it)->getNickname(), message);
	}
}

void Server::sendMessageToUser(
	const User *senderUser,
	const std::string& targetNickname,
	const std::string& message
){
	try { 	// Find User by Nickname
		User &targetUser = getUserByNickname(*senderUser, targetNickname);

		// Send message to User
		std::string privMessage =
			":" + senderUser->getUserIdentifier() +
			" PRIVMSG " + targetNickname +
			" :" + message + "\r\n";

		sendMessage(targetUser.getFd(), privMessage);

	} 	// Check if exists
	catch (const std::runtime_error& e){
		std::cerr << "PRIVMSG: " << e.what() << std::endl;
	}
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
		const StringSet& modes = ch.getChannelModes();
		for (
			StringSet::const_iterator it = modes.begin();
			it != modes.end();
			++it
		) {
			std::cout << WHITE << *it << " " RESET;
		}
		std::cout << "\n\n";
	}

	std::cout << BOLD CYAN "\n==== Users ====" RESET "\n";
	for (UserListConstIterator uIt = users.begin(); uIt != users.end(); ++uIt) {
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
