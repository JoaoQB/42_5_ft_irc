/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:19 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/16 17:25:49 by jqueijo-         ###   ########.fr       */
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

		// Message Handling
		void handleRawMessage(int fd, const char* buffer);

		// Command Handlers
		void handleJoinCommand(int fd, const std::string& rawMessage);

	private:
		// Server State
		std::string name;
		int serverSocketFd;
		int serverPort;
		std::string serverPassword;
		static bool signal;

		// Data Containers
		std::list<User> users;
		std::vector<struct pollfd> pollFds;
		std::list<Channel> channels;

		// Channel Utilities
		Channel& getChannel(const std::string& channelName);
		bool channelExists(const std::string& channelName) const;
		void createChannel(int userFd, const std::string& channelName, const std::string& channelKey);
		void addUserToChannel(int userFd, const std::string& channelName, const std::string& channelKey);
		void sendJoinReplies(const User* user, const Channel* channel);
		void broadcastJoin(const User* user, const Channel* channel);
		void sendChannelTopic(const User* user, const Channel* channel);
		void sendChannelUsersAndSetat(const User* user, const Channel* channel);

		// User Utilities
		User& getUser(int fd);
		void sendMessage(int userFd, const std::string &message);
		void sendNumericReply(
			const User* user,
			 NumericReply numericCode,
			const std::string& command,
			const std::string& message
		);
};

#endif
