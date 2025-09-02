//
//
//

#ifndef USER_HPP
# define USER_HPP

# include "./Common.hpp"
# include "Channel.hpp"

class User {
	public:
		User();

		int getFd() const;
		const std::string& getIpAddress() const;
		const std::string& getRealname() const;
		const std::string& getNickname() const;
		const std::string& getUsername() const;
		const std::string& getPassword() const;
		const std::string& getUserIdentifier() const;
		bool isRegistered(void) const;
		bool hasChannel(const Channel* channel) const;
		std::vector<Channel*>& getChannels();
		const std::vector<Channel*>& getChannels() const;

		void setFd(int fd);
		void setIpAddress(const std::string& ipAddr);
		void setRealname(std::string realname);
		void setUsername(std::string username);
		void setNickname(std::string nickname);
		void setPassword(std::string password);
		void setRegistered(bool registered);
		void setUserIdentifier();

		void addChannel(Channel* channel);
		void removeChannel(Channel* channel);

	private:
		int fd;
		std::string ipAddress;
		std::string realname;
		std::string username;
		std::string nickname;
		std::string password;
		std::string userIdentifier;
		bool registered;

		std::vector<Channel*> userChannels;
};

#endif
