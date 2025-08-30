/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dpetrukh <dpetrukh@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:19 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/08/29 12:08:11 by dpetrukh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "./Common.hpp"
# include "./User.hpp"
# include "./Channel.hpp"
# include "./Parser.hpp"

class Server {
	public:
		static const int BUFFER_SIZE = 1024;

		// Constructor
		Server();

		// Lifecycle & Core Setup
		void serverInit(const std::string& port, const std::string& password);
		void serverSocketCreate();
		static void signalHandler(int signum);
		void closeFds();
		void clearUser(int fd);

		// Connection Handling
		void acceptNewUser();
		void receiveNewData(int fd);
		void quitUser(int fd, bool quit, const std::string& quitMessage);

		void handleRawMessage(int fd, const char* buffer);

		// Command Handlers
		void handlePassCommand(User &user, std::string cmdParameters);
		void handleNickCommand(User &user, std::string cmdParameters);
		void handleUserCommand(User &user, std::string cmdParameters);
		void handleJoinCommand(User &user, const std::string& commandParams);
		void handleTopicCommand(User &user, const std::string& commandParams);
		void handleModeCommand(User &user, const std::string& commandParams);
		void handlePartCommand(User &user, const std::string& commandParams);
		void handleWhoCommand(User &user, const std::string& commandParams);
		void handleQuitCommand(User &user, const std::string& commandParams);

	private:
		// Server State
		std::string name;
		int serverSocketFd;
		int serverPort;
		std::string serverPassword;
		static bool signal;
		std::string serverCreationTime;
		std::string version;

		// Data Containers
		std::list<User> users;
		std::vector<struct pollfd> pollFds;
		std::list<Channel> channels;

		// Channel Utilities
		Channel& getChannel(User& targetUser, const std::string& channelName);
		bool channelExists(const std::string& channelName) const;
		void createChannel(User& creator, const std::string& channelName, const std::string& channelKey);
		void addUserToChannel(User& targetUser, const std::string& channelName, const std::string& channelKey);
		void sendJoinReplies(const User* user, const Channel* channel);
		void broadcastCommand(
			const User* user,
			const Channel* channel,
			const std::string& command,
			const std::string& message
		);
		void sendChannelTopic(const User* user, const Channel* channel);
		void sendChannelUsers(const User* user, const Channel* channel);
		void sendChannelSetAt(const User* user, const Channel* channel);
		void replyToWho(const User* user, const Channel* channel);
		void partUserFromChannel(User* user, Channel* channel);
		void removeChannel(Channel* channel);

		// User Utilities
		User& getUser(int fd);
		bool nicknameExists(const std::string& nickname) const;
		void registerUser(User &user);
		void disconnectUserFromAllChannels(User* user, bool quit, const std::string& quitMessage);
		void removeUser(int fd);

		// Message to Clients
		void sendMessage(int userFd, const std::string &message);
		void sendNumericReply(
			const User* user,
			NumericReply numericCode,
			const std::string& message
		);

		void debugPrintUsersAndChannels() const;
};

#endif
