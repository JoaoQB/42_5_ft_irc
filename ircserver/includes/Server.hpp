/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 10:58:19 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/03 12:51:25 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "./Client.hpp"
# include <iostream>
# include <string>
# include <cstring>
# include <vector>
# include <sys/socket.h> // socket()
# include <sys/types.h> // socket type definitions
# include <netinet/in.h> // sockaddr_in
# include <fcntl.h> // Fcntl()
# include <unistd.h> // close()
# include <arpa/inet.h> // inet_ntoa()
# include <poll.h> // poll()
# include <csignal> // signal()

#define RED "\033[1;31m" // red color
#define WHI "\033[0;37m" // white color
#define GRE "\033[1;32m" // green color
#define YEL "\033[1;33m" // yellow color

class Server {
	public:
		Server();

		void serverInit();
		void serverSocketCreate();
		void acceptNewClient();
		void receiveNewData(int fd);

		static void signalHandler(int signum);

		void closeFds();
		void clearClients(int fd);

	private:
		int serverPort;
		int serverSocketFd;
		static bool signal;
		std::vector<Client> clients;
		std::vector<struct pollfd> pollFds;
};

#endif
