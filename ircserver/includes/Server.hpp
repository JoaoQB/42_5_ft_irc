//
//
//

#ifndef SERVER_HPP
# define SERVER_HPP

# include "./Common.hpp"
# include "./User.hpp"
# include "./Channel.hpp"
# include "./Parser.hpp"

class Server {
	public:
		std::string name;
		Server();

		void serverInit(const std::string& port, const std::string& password);
		static void signalHandler(int signum);
		void closeFds();

		void broadcastCommand(
			const std::string& identifier,
			const Channel* channel,
			const std::string& command,
			const std::string& message
		);

		void handleServerShutdown();

	private:
		static const int BUFFER_SIZE = 1024;

		// Server State
		int serverSocketFd;
		int serverPort;
		std::string serverPassword;
		static bool signal;
		std::string serverCreationTime;
		std::string version;

		// Data Containers
		BufferMap rawMessageBuffers;
		std::list<User> users;
		std::vector<struct pollfd> pollFds;
		std::list<Channel> channels;

		// Lifecycle & Core Setup
		void serverSocketCreate();

		// Connection Handling
		void acceptNewUser();
		void receiveNewData(int fd);
		void processPendingDisconnects();

		// Raw Message Handling
		bool hasMessageInBuffer(int targetFd) const;
		void processPendingMessages(int fd, const std::string& receivedMessage);
		void handleRawMessage(int fd, const std::string& rawMessage);

		// Command Handlers
		void handlePassCommand(User &user, std::string cmdParameters);
		void handleNickCommand(User &user, std::string cmdParameters);
		void handleUserCommand(User &user, std::string cmdParameters);
		void handleJoinCommand(User &user, const std::string& commandParams);
		void handlePrivMsgCommand(User &user, const std::string& commandParams);
		void handleInviteCommand(User &invitingUser, const std::string& commandParams);
		void handleTopicCommand(User &user, const std::string& commandParams);
		void handleModeCommand(User &user, const std::string& commandParams);
		void handlePartCommand(User &user, const std::string& commandParams);
		void handleQuitCommand(User &user, const std::string& commandParams);
		void handlePingQuery(User &user, const std::string& commandParams);
		void handleWhoQuery(User &user, const std::string& commandParams);

		// Channel Utilities
		Channel& getChannel(const User& targetUser, const std::string& channelName);
		bool channelExists(const std::string& channelName) const;
		void createChannel(User& creator, const std::string& channelName, const std::string& channelKey);
		void addUserToChannel(User& targetUser, const std::string& channelName, const std::string& channelKey);
		void sendJoinReplies(const User* user, const Channel* channel);
		void sendChannelTopic(const User* user, const Channel* channel);
		void sendChannelUsers(const User* user, const Channel* channel);
		void sendChannelSetAt(const User* user, const Channel* channel);
		void replyToChannelWho(const User* user, const Channel* channel);
		void partUserFromChannel(
			User* user, Channel* channel,
			bool quit,
			const std::string& reason
		);
		void disconnectUserFromAllChannels(
			User* user,
			bool quit,
			const std::string& quitReason
		);
		void removeChannel(Channel* channel);

		// Channel Mode Utilities
		void replyToChannelMode(const User* user, const Channel* channel);
		void setChannelMode(
			const User* user,
			Channel* channel,
			const std::string& parameters
		);
		void setAndBroadcastModes(
			const User* user,
			Channel* channel,
			const StringMap& modesWithParams
		);
		bool setMode(
			const User* user,
			Channel* channel,
			const std::string& mode,
			const std::string& param
		);
		bool handleOperatorMode(
			const User* user,
			Channel* channel,
			const char& sign,
			const std::string& param
		);

		// User Utilities
		User& getUserByFd(int fd);
		User& getUserByNickname(const User& targetUser, const std::string& nickname);
		bool userExists(int fd) const;
		bool nicknameExists(const std::string& nickname) const;
		void registerUser(User &user);
		void replyToUserWho(const User* askingUser, const User* targetUser);
		void clearUser(int fd);
		void disconnectUser(int fd);

		// Message to Clients
		void sendMessage(int userFd, const std::string &message);
		void sendNumericReply(
			const User* user,
			NumericReply numericCode,
			const std::string& message
		);

		// Message to Users / Channels
		void processSingleTargetMessage(
			const User *senderUser,
			const std::string& target,
			const std::string& message
		);
		void sendMessageToChannel(
			const User* senderUser,
			const Channel &targetChannel,
			const std::string& message
		);
		void sendMessageToUser(
			const User *senderUser,
			const std::string& targetUsername,
			const std::string& message
		);

		void debugPrintUsersAndChannels() const;
};

#endif
