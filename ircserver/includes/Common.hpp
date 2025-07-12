/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Common.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:32:40 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/11 13:56:14 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_HPP
# define COMMON_HPP

class User;
class Server;
class Parser;
class Channel;

# include <iostream>
# include <string>
# include <cstring>
# include <algorithm>
# include <vector>
# include <map>
# include <list>
# include <sys/socket.h> // socket()
# include <sys/types.h> // socket type definitions
# include <netinet/in.h> // sockaddr_in
# include <fcntl.h> // Fcntl()
# include <unistd.h> // close()
# include <arpa/inet.h> // inet_ntoa()
# include <poll.h> // poll()
# include <csignal> // signal()
# include <cstdlib> // atoi

#define RED "\033[1;31m" // red color
#define WHI "\033[0;37m" // white color
#define GRE "\033[1;32m" // green color
#define YEL "\033[1;33m" // yellow color

enum CommandType {
	CMD_PASS,
	CMD_NICK,
	CMD_USER,
	CMD_JOIN,
	CMD_PRIVMSG,
	CMD_KICK,
	CMD_INVITE,
	CMD_TOPIC,
	CMD_MODE,
	CMD_PART,
	CMD_QUIT,
	CMD_UNKNOWN
};

typedef std::vector<struct pollfd>::iterator pollIterator;
typedef std::vector<User>::iterator UserIterator;
typedef std::map<std::string, CommandType> CommandMap;
typedef CommandMap::const_iterator CommandMapConstIterator;
typedef std::map<std::string, std::string> stringMap;
typedef std::map<std::string, std::string>::const_iterator stringMapConstIterator;
typedef std::string::size_type stringSizeT;

#endif
