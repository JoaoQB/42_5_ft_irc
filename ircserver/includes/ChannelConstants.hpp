//
//
//

#ifndef CHANNELCONSTANTS_HPP
# define CHANNELCONSTANTS_HPP

#include <string>

const std::string TOPIC_MODE = "t";
const std::string INVITE_MODE = "i";
const std::string LIMIT_MODE = "l";

const std::string CHANNEL_MODES[] = {
	TOPIC_MODE,
	INVITE_MODE,
	LIMIT_MODE,
};

const size_t CHANNEL_MODE_SIZE = sizeof(CHANNEL_MODES) / sizeof(CHANNEL_MODES[0]);

#endif
