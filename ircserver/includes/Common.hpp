/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Common.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jqueijo- <jqueijo-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:32:40 by jqueijo-          #+#    #+#             */
/*   Updated: 2025/07/09 13:39:56 by jqueijo-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERS_HPP
# define HEADERS_HPP

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

typedef std::vector<struct pollfd>::iterator pollIterator;
typedef std::vector<User>::iterator UserIterator;

#endif
