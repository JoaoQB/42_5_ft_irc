/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 12:21:39 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/06/30 15:21:47 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

// int main(int argc, char** argv) {
// 	if (argc != 3) {
// 		std::cerr << "Error: invalid input, "
// 			<< "please provide a port and password as arguments.\n"
// 			<< "Usage: './ircserv <port> <password> [...]'\n"
// 			<< "Example: './ircserv 5 9 1 20 3'" << std::endl;
// 		return 1;
// 	}
// 	return 0;
// }

int main(void) {
	Server server;

	std::cout << "---- SERVER ----" << std::endl;
	try {
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		server.serverInit();
	}
	catch(const std::exception& e) {
		server.closeFds();
		std::cerr << "Error: " << e.what() << std::endl;
	}
	std::cout << "The Server Closed!" << std::endl;
}
