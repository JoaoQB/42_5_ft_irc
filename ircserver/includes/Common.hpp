//
//
//

#ifndef COMMON_HPP
# define COMMON_HPP

class User;
class Server;
class Parser;
class Channel;

# include <iostream>
# include <sstream>
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
# include <sstream> //isstringstream
# include <ctime> // getDayAndTime()

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
	CMD_UNKNOWN,
	CAP
};

enum NumericReply {
	RPL_WELCOME = 001,
	RPL_YOURHOST = 002,
	RPL_CREATED = 003,
	RPL_MYINFO = 004,
	RPL_ISUPPORT = 005,
	RPL_CREATIONTIME = 329,
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_TOPICWHOTIME = 333,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	ERR_NOSUCHCHANNEL = 403,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NOTONCHANNEL = 442,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_CHANNELISFULL = 471,
	ERR_INVITEONLYCHAN = 473,
	ERR_BADCHANNELKEY = 475,
	ERR_BADCHANMASK = 476,
	ERR_CHANOPRIVSNEEDED = 482
};

typedef std::vector<struct pollfd>::iterator PollIterator;

typedef std::list<User>::iterator UserListIterator;
typedef std::list<User>::const_iterator UserListConstIterator;
typedef std::vector<User*>::iterator UserVectorIterator;
typedef std::vector<User*>::const_iterator UserVectorConstIterator;

typedef std::map<std::string, CommandType> CommandMap;
typedef CommandMap::const_iterator CommandMapConstIterator;

typedef std::map<std::string, std::string> StringMap;
typedef StringMap::const_iterator StringMapConstIterator;

typedef std::list<Channel>::iterator ChannelListIterator;
typedef std::list<Channel>::const_iterator ChannelListConstIterator;
typedef std::vector<Channel*>::iterator ChannelVectorIterator;
typedef std::vector<Channel*>::const_iterator ChannelVectorConstIterator;

typedef std::string::size_type StringSizeT;

#endif
