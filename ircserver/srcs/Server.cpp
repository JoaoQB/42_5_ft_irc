/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpetrukh <dpetrukh@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:14 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:18:14 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::signal = false;

Server::Server()
	: name("ft_irc")
	, serverSocketFd(-1)
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

	//rawMessage = "PASS mypassword"
	std::string command = Parser::extractCommand(rawMessage);
	// command = "PASS"
	CommandType cmd = Parser::getCommandType(command);
	// cmd = 1
	std::string params = Parser::extractParams(rawMessage, command);

	User &user = getUser(fd);
	// // TODO
	// if (!Parser::isAuth(user, cmd)) {
	// 	std::cout << "ERR_NOTREGISTERED (451)" << std::endl;
	// 	return;
	// }
	// Usuário está retrito a fazer outros comandos enquanto que não está registrado no servidor
	if (cmd != CMD_PASS && cmd != CMD_USER && cmd != CMD_NICK && !user.isRegistered()) {
		std::cout << "ERR_NOTREGISTERED (451)" << std::endl;
		return;
	}
	switch (cmd) {
		case CMD_PASS: {
			cmdPass(user, params);
			break;
		}
		case CMD_NICK: {
			cmdNick(user, params);
			break;
		}
		case CMD_USER:
			cmdUser(user, params);
			break;
		case CMD_JOIN:
			handleJoinCommand(fd, params);
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

void Server::handleJoinCommand(int fd, const std::string& rawMessageParams) {
	// const StringSizeT commandPrefixLength = 5; // Account for space after 'JOIN'
	User &user = getUser(fd);
	if (rawMessageParams.empty()) {
		Parser::ft_error("empty JOIN command");
		std::string needMoreParams = "JOIN :Not enough parameters";
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, needMoreParams);
		return;
	}
	bool isJoin0Command = rawMessageParams == "0";
	if (isJoin0Command) {
		disconnectUserFromAllChannels(&user);
		return;
	}

	StringSizeT keyStart = rawMessageParams.find(' ');
	std::string channelNames = Parser::extractChannelNames(rawMessageParams, keyStart);
	std::string channelKeys = Parser::extractChannelKeys(rawMessageParams, keyStart);
	StringMap channelsWithKeys = Parser::mapChanneslWithKeys(channelNames, channelKeys);

	for (StringMapConstIterator it = channelsWithKeys.begin();
		it != channelsWithKeys.end();
		++it
	) {
		const std::string& channelName = it->first;
		const std::string& key = it->second;
		if (!Parser::validateChannelName(channelName)) {
			Parser::ft_error("invalid Channel name");
			std::string badChannelName = channelName
				+ " :Bad Channel Mask";
			sendNumericReply(&user, ERR_BADCHANMASK, badChannelName);
			continue;
		}
		try {
			if (channelExists(channelName)) {
				addUserToChannel(user, channelName, key);
			} else {
				createChannel(user, channelName, key);
			}
		} catch (const std::exception& e) {
			std::cerr << "Failed to add user to channel: " << e.what() << std::endl;
		}
	}
}

Channel& Server::getChannel(User& targetUser, const std::string& channelName) {
	for (
		ChannelListIterator it = this->channels.begin() ;
		it != this->channels.end() ;
		++it
	) {
		if (it->getName() == channelName) {
			return *it;
		}
	}
	std::string noSuchChannel = channelName
		+ " :No such channel";
	sendNumericReply(&targetUser, ERR_NOSUCHCHANNEL, noSuchChannel);
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
	User& creator,
	const std::string& channelName,
	const std::string& channelKey
) {
	Channel newChannel;
	this->channels.push_back(newChannel);
	Channel* joinedChannel = &this->channels.back();

	joinedChannel->setName(channelName);
	if (!channelKey.empty()) {
		joinedChannel->setPassword(channelKey);
	}
	joinedChannel->addUser(&creator);
	joinedChannel->addOperator(&creator);

	creator.addChannel(joinedChannel);

	sendJoinReplies(&creator, joinedChannel);
	std::cout << "Channel " << channelName << " created by user " << creator.getFd() << "\n";
}

void Server::addUserToChannel(
	User& targetUser,
	const std::string& channelName,
	const std::string& channelKey
) {
	Channel& targetChannel = getChannel(targetUser, channelName);
	if (targetChannel.hasUser(&targetUser)) {
		std::cerr << "User " << targetUser.getFd() << " is already in channel " << channelName << std::endl;
		return;
	}
	if (targetChannel.isFull()) {
		Parser::ft_error("channel full");
		std::string channelIsFull = targetChannel.getName()
			+ " :Cannot join channel (+l)";
		sendNumericReply(&targetUser, ERR_CHANNELISFULL, channelIsFull);
		return ;
	}
	if (targetChannel.isInviteOnly()) {
		Parser::ft_error("channel is invite only");
		std::string inviteOnly = targetChannel.getName()
			+ " :Cannot join channel (+i)";
		sendNumericReply(&targetUser, ERR_INVITEONLYCHAN, inviteOnly);
		return ;
	}
	if (targetChannel.requiresPassword() &&
		targetChannel.getPassword() != channelKey
	) {
		Parser::ft_error("channel password is incorrect");
		std::string badChannelKey = targetChannel.getName()
			+ " :Cannot join channel (+k)";
		sendNumericReply(&targetUser, ERR_BADCHANNELKEY, badChannelKey);
		return;
	}
	targetChannel.addUser(&targetUser);
	targetUser.addChannel(&targetChannel);
	sendJoinReplies(&targetUser, &targetChannel);
	std::cout << "User " << targetUser.getFd() << " added to channel " << channelName << std::endl;
}

void Server::sendJoinReplies(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	broadcastCommand(user, channel, "JOIN");
	sendChannelTopic(user, channel);
	sendChannelUsers(user, channel);
	sendChannelSetAt(user, channel);
}

void Server::broadcastCommand(const User* user, const Channel* channel, const std::string& command) {
	if (!user || !channel) {
		return ;
	}
	std::string joinMessage = ":" + user->getUserIdentifier()
		+ " " + command
		+ " " + channel->getName();
	for (
		UserVectorConstIterator it = channel->getUsers().begin();
		it != channel->getUsers().end();
		++it
	) {
		sendMessage((*it)->getFd(), joinMessage);
	}
}

void Server::sendChannelTopic(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	if (!channel->hasTopic()) {
		std::string noTopicMessage = channel->getName()
			+ " :" + "No topic is set";
		sendNumericReply(user, RPL_NOTOPIC, noTopicMessage);
		return;
	}
	std::string topicMessage = channel->getName()
		+ " :" + channel->getTopic();
	sendNumericReply(user, RPL_TOPIC, topicMessage);

	std::string topicWhoTimeMessage = channel->getName()
		+ " " + channel->getTopicSetter()
		+ " " + channel->getTopicCreationTime();
	sendNumericReply(user, RPL_TOPICWHOTIME, topicWhoTimeMessage);
}

void Server::sendChannelUsers(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	std::string channelUsers;
	for (
		UserVectorConstIterator it = channel->getUsers().begin();
		it != channel->getUsers().end();
		++it
	) {
		std::string isOperatorPrefix = channel->isOperator(*it) ? "@" : "";
		channelUsers += isOperatorPrefix + (*it)->getNickname() + " ";
	}
	std::string usersMessage = "= " + channel->getName()
		+ " :" + channelUsers;
	sendNumericReply(user, RPL_NAMREPLY, usersMessage);

	std::string endOfNames = channel->getName()
		+ " :End of /NAMES list";
	sendNumericReply(user, RPL_ENDOFNAMES, endOfNames);
}

void Server::sendChannelSetAt(const User* user, const Channel* channel) {
	if (!user || !channel) {
		return ;
	}

	std::string setAt = channel->getName()
		+ " " + channel->getCreationTime();
	sendNumericReply(user, RPL_CREATIONTIME, setAt);
}

/*
// This function can only be called on a copy of a user's <channel*> vector,
// because it modifies user->userChannels.
*/
void Server::partUserFromChannel(User* user, Channel* channel) {
	if (!user || !channel) {
		return ;
	}
	broadcastCommand(user, channel, "PART");
	channel->removeUser(user);
	user->removeChannel(channel);
	if (channel->isEmpty()) {
		this->removeChannel(channel);
		return;
	}
}

void Server::removeChannel(Channel* channel) {
	if (!channel || !channel->isEmpty()) {
		return;
	}

	for (std::list<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
		if (&(*it) == channel) {
			std::cout << "Removing empty channel: " << it->getName() << std::endl;
			channels.erase(it);
			return;
		}
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

void Server::disconnectUserFromAllChannels(User* user) {
	if (!user) {
		return;
	}
	std::vector<Channel*> userChannelCopies = user->getChannels();
	for (
		ChannelVectorIterator channelIt = userChannelCopies.begin();
		channelIt != userChannelCopies.end();
		++channelIt
	) {
		partUserFromChannel(user, *channelIt);
	}
	user->getChannels().clear();
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

	if (!message.empty())
		reply += " " + message;

	sendMessage(user->getFd(), reply);
}

void Server::cmdPass(User &user, std::string cmdParameters){
	// std::cout << "Command parameters: " << cmdParameters << std::endl;

	if (cmdParameters.empty()) {
		std::cout << "Error 461 needmoreparams" << std::endl;
	}

	// 1 - Se parametros vaziu ou Se Nick ou User já tiverem algo, erro
	if (!user.getNickName().empty() || !user.getUserName().empty() || !user.getPassword().empty()) {
		//sendError(user.getFd(), "462", "You may not reregister");
		std::cout << "Error 462 alreadyregistered" << std::endl;
		return;
	}

	// 2 - Se comeca com ":", remover o ":" e aceitar espaços
	std::string password;

	if (!cmdParameters.empty() && cmdParameters[0] == ':')
		password = cmdParameters.substr(1);
	else { // Se NÃO começa ":", substr até primeiro espaço ou fim.
		size_t spacePos = cmdParameters.find(' ');
		if (spacePos != std::string::npos)
			password = cmdParameters.substr(0, spacePos);
		else
			password = cmdParameters;
	}

	// 3 - Remover o \r e \n no fim
	password = Parser::trimCRLF(password);

	// 4 - Verificar se a password coicide com o servidor, se errada 464 ERR_PASSWDMISMATCH.
	// std::cout << "Password result: " << password << " length: " << password.length() << std::endl;
	// std::cout << "Password server: " << this->serverPassword << " length: " << this->serverPassword.length() << std::endl;
	if (password != this->serverPassword) {
		std::cout << "Error 464 passmismatch" << std::endl;
		return ;
	}

	// 5 - Adicionar ao user.password
	user.setPassword(password);
	std::cout << "✅ User Password Registered Successfully: " << user.getPassword() << std::endl;
}

// TODO Verificar se forem múltiplos parâmetros, aceitar só o primeiro
void Server::cmdNick(User &user, std::string cmdParameters) {
	// Se password é NULL erro
	if (user.getPassword().empty()) {
		std::cout << "Tens de ter a PASS primeiro antes de passar o user" << std::endl;
	}

	// Não pode ser vaziu
	if (cmdParameters.empty()) {
		std::cout << "Error 431 nonicknamegiven" << std::endl;
		return ;
	}

	// Remover o \r e \n no fim
	cmdParameters = Parser::trimCRLF(cmdParameters);

	// AQUI <--
	std::string nickname = Parser::extractFirstParam(cmdParameters);

	// Proteção caracteres especiais && nickname não pode ser outro comando como NICK PASS JOIN...
	if (!Parser::validateNickname(nickname)) {
		std::cout << "ERR_ERRONEUSNICKNAME (432)" << std::endl;
		return ;
	}

	// // Se o nickname já está existe na mesma network
	// for (size_t i = 0; i < users.size(); i++) {
	// 	if (users[i].getNickName() == nickname) {
	// 		std::cout << "ERR_NICKNAMEINUSE (433)" << std::endl;
	// 		return ;
	// 	}
	// }

	//TODO
	//Check Username exists in server users

	// Adicionar nickname ao user
	user.setNickName(nickname);
	std::cout << "✅ User Nickname Registered Successfully: " << user.getNickName() << std::endl;
	turnRegistrationOn(user);
}

// USER dpetrukh 8 * :Dinis Petrukha : USER <username> <hostname> <servername> :<realname>
void Server::cmdUser(User &user, std::string cmdParameters){
	// Se password é NULL erro
	if (user.getPassword().empty()) {
		std::cout << "Tens de ter a PASS primeiro antes de passar o user" << std::endl;
		return;
	}

	// Se já é registrado e usar USER novamente, devolve ERR_ALREADYREGISTERED (462)
	if (user.isRegistered() == true) {
		std::cout << "ERR_ALREADYREGISTERED (462)" << std::endl;
		return ;
	}


	// Remover \r\v
	cmdParameters = Parser::trimCRLF(cmdParameters);

	// Separar em tokens
	std::istringstream iss(cmdParameters);
	std::string username, hostname, servername, realname;

	if (!(iss >> username >> hostname >> servername)) {
		// Se nem username nem os dois params obrigatórios vierem
		std::cout << "ERR_NEEDMOREPARAMS (461)" << std::endl;
		return;
	}

	std::getline(iss, realname);

	if (!realname.empty() && realname[0] == ' ')
		realname.erase(0, 1);
	if (!realname.empty() && realname[0] == ':')
		realname.erase(0, 1);

	if (username.empty()) {
		std::cout << "ERR_NEEDMOREPARAMS (461)" << std::endl;
		return;
	}

	if (realname.empty()) {
		std::cout << "ERR_NEEDMOREPARAMS (461)" << std::endl;
		return;
	}

	user.setUserName(username);
	user.setRealName(realname);
	std::cout << "✅ User Username + Realname Registered Successfully: " << user.getUserName() << " " << user.getRealName() << std::endl;
	turnRegistrationOn(user);
}

void Server::turnRegistrationOn(User &user) {
	if (user.getPassword().empty() ||
		user.getNickName().empty() ||
		user.getUserName().empty() ||
		user.getRealName().empty()) {
			return ;
	}
	if (!user.isRegistered()) {
		user.setRegistered(true); //passa para registrado

		std::cout << "🥳 Client " << user.getNickName()
			<< " fully registred!" << std::endl;
	}
}
