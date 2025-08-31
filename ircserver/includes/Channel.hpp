//
//
//

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "./Common.hpp"
# include "./User.hpp"
# include "./Parser.hpp"

class Channel {
	public:
		Channel();

		// Getters
		const std::string& getName() const;
		const std::string& getPassword() const;
		const std::string& getTopic() const;
		const std::string& getTopicSetter() const;
		const std::string& getTopicCreationTime() const;
		const std::string& getCreationTime() const;
		const std::vector<User*>& getUsers() const;

		// Boolean Accessors
		bool isFull() const;
		bool isEmpty() const;
		bool requiresPassword() const;
		bool isInviteOnly() const;
		bool isOperator(const User* user) const;
		bool hasTopic() const;
		bool isTopicProtected() const;
		bool hasUser(const User* user) const;

		// Setters
		void setName(const std::string& channelName);
		void setPassword(const std::string& key);
		void setTopic(const User* user, const std::string& message);

		// Actions
		void addUser(User* user);
		void addOperator(User* user);
		void removeUser(User* user);
		void deleteTopic();

	private:
		std::string name;
		std::string password;
		std::string channelCreationTime;
		std::string topic;
		std::string topicSetter;
		std::string topicCreationTime;

		bool hasPassword;
		bool full;

		int channelLimit;
		int usersInChannel;

		std::vector<User*> channelUsers;
		std::vector<User*> channelOperators;
		std::vector<std::string> channelModes;
};

#endif
