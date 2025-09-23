//
//
//

#ifndef CHANNELCONSTANTS_HPP
# define CHANNELCONSTANTS_HPP

#include <string>

const std::string REGISTRATION_MODE = "r";
const std::string OPERATOR_MODE = "o";
const std::string TOPIC_MODE = "t";
const std::string INVITE_MODE = "i";
const std::string LIMIT_MODE = "l";
const std::string PASSWORD_MODE = "k";

const std::string CHANNEL_MODES[] = {
	OPERATOR_MODE,
	TOPIC_MODE,
	INVITE_MODE,
	LIMIT_MODE,
	PASSWORD_MODE
};

const size_t CHANNEL_MODES_SIZE = sizeof(CHANNEL_MODES) / sizeof(CHANNEL_MODES[0]);

#endif
