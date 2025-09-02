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
		Server();

		void serverInit(const std::string& port, const std::string& password);
		static void signalHandler(int signum);
		void closeFds();

	private:
		static const int BUFFER_SIZE = 1024;

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

		// Lifecycle & Core Setup
		void serverSocketCreate();
		void clearUserFromPoll(int fd);

		// Connection Handling
		void acceptNewUser();
		void receiveNewData(int fd);
		void handleRawMessage(int fd, const char* buffer);

		// Command Handlers
		void handlePassCommand(User &user, std::string cmdParameters);
		void handleNickCommand(User &user, std::string cmdParameters);
		void handleUserCommand(User &user, std::string cmdParameters);
		void handleJoinCommand(User &user, const std::string& commandParams);
		void handleTopicCommand(User &user, const std::string& commandParams);
		void handleWhoQuery(User &user, const std::string& commandParams);

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
		void replyToChannelWho(const User* user, const Channel* channel);
		void partUserFromChannel(User* user, Channel* channel);
		void removeChannel(Channel* channel);

		// User Utilities
		User& getUserByFd(int fd);
		User& getUserByNickname(const std::string& nickname);
		bool nicknameExists(const std::string& nickname) const;
		void registerUser(User &user);
		void replyToUserWho(const User* askingUser, const User* targetUser);
		void disconnectUserFromAllChannels(User* user);

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
