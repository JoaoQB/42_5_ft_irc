//
//
//

#include "../includes/Server.hpp"

void Server::handlePassCommand(User &user, std::string cmdParameters){
	// std::cout << "Command parameters: " << cmdParameters << std::endl;
	// TODO tocar para um erro
	std::string cmd = "PASS";
	if (cmdParameters.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, cmd + " :Not enough parameters");
	}

	// 1 - Se parametros vaziu ou Se Nick ou User já tiverem algo, erro
	if (!user.getNickname().empty() || !user.getUsername().empty() || !user.getPassword().empty()) {
		sendNumericReply(&user, ERR_ALREADYREGISTERED, " :You may not reregister");
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

	// 3 - Verificar se a password coicide com o servidor, se errada 464 ERR_PASSWDMISMATCH.
	if (password != this->serverPassword) {
		sendNumericReply(&user, ERR_PASSWDMISMATCH, " :Password incorrect");
		return ;
	}

	// 4 - Adicionar ao user.password
	user.setPassword(password);
	std::cout << "[DEBUG]✅ User Password Registered Successfully: " << user.getPassword() << RESET << std::endl;
}

// TODO Verificar se forem múltiplos parâmetros, aceitar só o primeiro
void Server::handleNickCommand(User &user, std::string cmdParameters) {
	//TODO descontectar usuário caso passe nick antes da pass.
	if (user.getPassword().empty()) {
		sendNumericReply(&user, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}

	// Não pode ser vaziu
	if (cmdParameters.empty()) {
		sendNumericReply(&user, ERR_PASSWDMISMATCH, " :No nickname given");
		return ;
	}

	std::string nickname = Parser::extractFirstParam(cmdParameters);

	// Proteção caracteres especiais && nickname não pode ser outro comando como NICK PASS JOIN...
	if (!Parser::validateNickname(nickname)) {
		sendNumericReply(&user, ERR_ERRONEUSNICKNAME, nickname + " :Erroneus nickname");
		return ;
	}

	if (nicknameExists(nickname)) {
		std::cout << "[DEBUG] nickname:" + nickname << std::endl;
		sendNumericReply(&user, ERR_NICKNAMEINUSE, nickname + " :Nickname is already in use");
			return ;
	}

	// Adicionar nickname ao user
	user.setNickname(nickname);
	std::cout << "[DEBUG]✅ User Nickname Registered Successfully: " << user.getNickname() << RESET << std::endl;
	registerUser(user);
}

// USER dpetrukh 8 * :Dinis Petrukha : USER <username> <hostname> <servername> :<realname>
void Server::handleUserCommand(User &user, std::string cmdParameters){
	//TODO descontectar usuário caso passe user antes da pass.
	std::string cmd = "USER";

	if (user.getPassword().empty()) {
		sendNumericReply(&user, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}

	// Se já é registrado e usar USER novamente, devolve ERR_ALREADYREGISTERED (462)
	if (user.isRegistered() == true) {
		sendNumericReply(&user, ERR_ALREADYREGISTERED, " :You may not reregister");
		return ;
	}

	// Separar em tokens
	std::istringstream iss(cmdParameters);
	std::string username, hostname, servername, realname;

	if (!(iss >> username >> hostname >> servername)) {
		// Se nem username nem os dois params obrigatórios vierem
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, cmd + " :Not enough parameters");
		return;
	}

	std::getline(iss, realname);

	if (!realname.empty() && realname[0] == ' ')
		realname.erase(0, 1);
	if (!realname.empty() && realname[0] == ':')
		realname.erase(0, 1);

	if (username.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, cmd + " :Not enough parameters");
		return;
	}

	if (realname.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, cmd + " :Not enough parameters");
		return;
	}

	user.setUsername(username);
	user.setRealname(realname);
	std::cout << "[DEBUG]✅ User Username + Realname Registered Successfully: " << user.getUsername() << RESET << " " << user.getRealname() << std::endl;
	registerUser(user);
}

void Server::handleJoinCommand(User &user, const std::string& commandParams) {
	if (commandParams.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return;
	}
	bool isJoin0Command = commandParams == "0";
	if (isJoin0Command) {
		disconnectUserFromAllChannels(&user, false, "");
		return;
	}

	std::string channelNames = Parser::extractFirstParam(commandParams);
	std::string channelKeys = Parser::extractSecondParam(commandParams);
	StringMap channelsWithKeys = Parser::mapChanneslWithKeys(channelNames, channelKeys);

	for (StringMapConstIterator it = channelsWithKeys.begin();
		it != channelsWithKeys.end();
		++it
	) {
		const std::string& channelName = it->first;
		const std::string& key = it->second;
		if (!Parser::validateChannelName(channelName)) {
			Parser::ft_error("invalid Channel name");
			sendNumericReply(&user, ERR_BADCHANMASK, channelName + " :Bad Channel Mask");
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

void Server::handlePrivMsgCommand(User &user, const std::string& commandParams) {
	// commandParams = dpetrukh,joao :Hey guys
	//Separar os parâmetros
	std::string cmd = "PRIVMSG";
	std::istringstream iss(commandParams);
	std::string targets, message;

	// Validar a existência de parâmetros
	if (!(iss >> targets)) { //PRIVMSG <sem nada>
		sendNumericReply(&user, ERR_NORECIPIENT, ":No recipient given (" + cmd + ")");
		return;
	}

	std::getline(iss, message); // agarramos na mensagem completa com os espacos

	if (!message.empty() && message[0] == ' ')
		message.erase(0, 1);

	// Se começa com ':', remove apenas o ':' inicial
	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);
	else {
		// Se não começa com ':', pega apenas até o primeiro espaço
		std::istringstream iss(message);
		std::string firstWord;
		iss >> firstWord;
		message = firstWord;
	}

	if (message.empty()) { // Empty message!
		sendNumericReply(&user, ERR_NOTEXTTOSEND, ":No text to send");
		return;
	}

	std::set<std::string> targetsSet = Parser::splitStringToSet(targets);

	for (std::set<std::string>::iterator it = targetsSet.begin(); it != targetsSet.end(); ++it) {
		processSingleTargetMessage(&user, *it, message);
	}
}

void Server::handleInviteCommand(User &invitingUser, const std::string& commandParams) {
	std::string command = "INVITE";
	std::string username = Parser::extractFirstParam(commandParams);
	std::string channelName = Parser::extractSecondParam(commandParams);
	if (username.empty() || channelName.empty()) {
		sendNumericReply(&invitingUser, ERR_NEEDMOREPARAMS, command + " :Not enough parameters");
		return;
	}
	try {
		User& targetUser = getUserByNickname(invitingUser, username);
		Channel& targetChannel = getChannel(invitingUser, channelName);
		if (!targetChannel.hasUser(&invitingUser)) {
			sendNumericReply(&invitingUser, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
			return;
		}
		if (targetChannel.isInviteOnly() && !targetChannel.isOperator(&invitingUser)) {
			sendNumericReply(
				&invitingUser,
				ERR_CHANOPRIVSNEEDED,
				targetChannel.getName() + " :You're not channel operator"
			);
			return;
		}
		if (targetChannel.hasUser(&targetUser)) {
			std::string replyMessage = targetUser.getNickname()
				+ " " + channelName
				+ " :is already on channel";
			sendNumericReply(&invitingUser, ERR_USERONCHANNEL, replyMessage);
			return;
		}

		std::string validInviteReply = targetUser.getNickname() + " " + channelName;
		sendNumericReply(&invitingUser, RPL_INVITING, validInviteReply);

		std::string inviteMessage = ":" + invitingUser.getUserIdentifier()
			+ " " + command
			+ " " + targetUser.getNickname()
			+ " " + channelName;
		sendMessage(targetUser.getFd(), inviteMessage);

		targetChannel.addInvitedUser(&targetUser);
	} catch (const std::exception& e) {
		std::cerr << command << ": " << e.what() << std::endl;
	}
}

void Server::handleTopicCommand(User &user, const std::string& commandParams) {
	if (commandParams.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return;
	}
	std::string channelName = Parser::extractFirstParam(commandParams);
	try {
		Channel& targetChannel = getChannel(user, channelName);
		if (!targetChannel.hasUser(&user)) {
			sendNumericReply(&user, ERR_NOTONCHANNEL, channelName + " :Not on channel");
			return;
		}
		std::string commandTopic = Parser::extractFromSecondParam(commandParams);
		std::cout << "[DEBUG] Command Topic is: " << "\n";
		if (commandTopic.empty()) {
			sendChannelTopic(&user, &targetChannel);
			return;
		}
		if (targetChannel.isTopicProtected() && !targetChannel.isOperator(&user)) {
			sendNumericReply(
				&user,
				ERR_CHANOPRIVSNEEDED,
				targetChannel.getName() + " :You're not channel operator"
			);
			return;
		}
		// Trim whitespaces and remove ":" separator
		std::string trimmedCommandTopic = Parser::trimWhitespace(commandTopic);
		if (!trimmedCommandTopic.empty() && trimmedCommandTopic[0] == ':') {
			trimmedCommandTopic = trimmedCommandTopic.substr(1);
		}
		// If empty string after ':', delete topic
		if (trimmedCommandTopic.empty()) {
			targetChannel.removeTopic();
		} else {
			targetChannel.setTopic(&user, trimmedCommandTopic);
		}
		std::string topicMessage = targetChannel.getName() + " :" + trimmedCommandTopic;
		broadcastCommand(user.getUserIdentifier(), &targetChannel, "TOPIC", topicMessage);
	} catch (const std::exception& e) {
		std::cerr << "TOPIC: " << e.what() << std::endl;
	}
}

void Server::handleModeCommand(User &user, const std::string& commandParams) {
	if (commandParams.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return;
	}
	const std::string mask = Parser::extractFirstParam(commandParams);
	const std::string parameters = Parser::extractFromSecondParam(commandParams);
	try {
		if (Parser::isValidChannelPrefix(*mask.begin())) {
			Channel& targetChannel = getChannel(user, mask);
			if (!targetChannel.hasUser(&user)) {
				sendNumericReply(&user, ERR_NOTONCHANNEL, mask + " :Not on channel");
				return;
			}
			// If "MODE <channel>" reply with active channel modes
			if (parameters.empty()) {
				replyToChannelMode(&user, &targetChannel);
				return;
			}
			// Else if "Mode <channel> <listable_mode>" reply with not supported or ignore?
			if (Parser::isTypeAMode(parameters)) {
				// const std::string unsupportedMode = parameters.substr(1);
				// sendNumericReply(
				// 	&user,
				// 	ERR_UNKNOWNMODE,
				// 	unsupportedMode + " :is unknown mode char to me"
				// );
				return;
			}
			// Else if "MODE <channel> <params>" set params
			setChannelMode(&user, &targetChannel, parameters);
			return;
		}
		//TODO handle user mode
		// User& targetUser = getUserByNickname(user, mask);
	} catch (const std::exception& e) {
		std::cerr << "MODE: " << e.what() << std::endl;
	}
}

void Server::handlePartCommand(User &user, const std::string& commandParams) {
	if (commandParams.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return;
	}
	std::string channelNames = Parser::extractFirstParam(commandParams);
	std::string reason = Parser::extractFromSecondParam(commandParams);
	std::list<std::string> channels = Parser::splitStringToList(channelNames, ",");
	for (
		std::list<std::string>::iterator chanIt = channels.begin();
		chanIt != channels.end();
		++chanIt
	) {
		try {
			Channel& targetChannel = getChannel(user, *chanIt);
			if (!targetChannel.hasUser(&user)) {
				sendNumericReply(&user, ERR_NOTONCHANNEL, *chanIt + " :Not on channel");
				return;
			}
			partUserFromChannel(&user, &targetChannel, false, reason);
		} catch (const std::exception& e) {
			std::cerr << "PART: " << e.what() << std::endl;
		}
	}
}

void Server::handleQuitCommand(User &user, const std::string& commandParams) {
	std::cout << commandParams << "\n";
	disconnectUserFromAllChannels(&user, true, commandParams);
	int userFd = user.getFd();
	clearUser(userFd);
	close(userFd);
}

void Server::handlePingQuery(User &user, const std::string& commandParams) {
	if (commandParams.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, "PING :Not enough parameters");
		return;
	}
	std::string reference = Parser::extractFirstParam(commandParams);
	std::string reply = "PONG " + reference;
	sendMessage(user.getFd(), reply);
}

void Server::handleWhoQuery(User &user, const std::string& commandParams) {
	if (commandParams.empty()) {
		sendNumericReply(&user, ERR_NEEDMOREPARAMS, "WHO :Not enough parameters");
		return;
	}
	std::string mask = Parser::extractFirstParam(commandParams);
	try {
		if (Parser::isValidChannelPrefix(*mask.begin())) {
			Channel& targetChannel = getChannel(user, mask);
			replyToChannelWho(&user, &targetChannel);
			return;
		}
		User& targetUser = getUserByNickname(user, mask);
		replyToUserWho(&user, &targetUser);
	} catch (const std::exception& e) {
		std::cerr << "WHO: " << e.what() << std::endl;
	}
}
