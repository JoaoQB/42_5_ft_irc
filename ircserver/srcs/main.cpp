//
//
//

#include "../includes/User.hpp"
#include "../includes/Server.hpp"

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Error: invalid input, "
			<< "please provide a port and password as arguments.\n"
			<< "Usage: './ircserv <port> <password>'\n"
			<< "Example: './ircserv 6667 mypassword'" << std::endl;
		return 1;
	}
	Server server;
	const std::string& port = argv[1];
	const std::string& password = argv[2];

	std::cout << "---- SERVER ----" << std::endl;
	try {
		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		signal(SIGHUP, Server::signalHandler);
		server.serverInit(port, password);
	}
	catch(const std::exception& e) {
		server.handleServerShutdown();
		std::cerr << "Error: " << e.what() << std::endl;
	}
	std::cout << "The Server Closed!" << std::endl;
}
