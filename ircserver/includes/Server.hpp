/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:19 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/10 09:59:56 by jqueijo-         ###   ########.fr       */
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

		Server();

		void serverInit(const std::string& port, const std::string& password);
		void serverSocketCreate();
		void acceptNewUser();
		void receiveNewData(int fd);

		static void signalHandler(int signum);

		void closeFds();
		void clearUsers(int fd);

		void handleRawMessage(const char* buffer);

	private:
		int serverSocketFd;
		int serverPort;
		std::string serverPassword;

		static bool signal;

		std::vector<User> users;
		std::vector<struct pollfd> pollFds;
		std::vector<Channel> channels;


};

#endif
