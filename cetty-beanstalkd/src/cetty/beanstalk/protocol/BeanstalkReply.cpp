/*
 * BeanstalkReply.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/protocol/BeanstalkReply.h>

namespace cetty {
namespace beanstalk {
namespace protocol {

const int BeanstalkReply::INSERTED = 0;
const int BeanstalkReply::BURIED = 1;
const int BeanstalkReply::USING = 2;
const int BeanstalkReply::RESERVED = 3;
const int BeanstalkReply::WATCHING = 4;
const int BeanstalkReply::KICKED = 5;
const int BeanstalkReply::FOUND = 6;
const int BeanstalkReply::OK = 7;

const std::string BeanstalkReply::inserted = "INSERTED";
const std::string BeanstalkReply::buried = "BURIED";
const std::string BeanstalkReply::rusing = "USING";
const std::string BeanstalkReply::reserved = "RESERVED";
const std::string BeanstalkReply::watching = "WATCHING";
const std::string BeanstalkReply::kicked = "KICKED";
const std::string BeanstalkReply::found = "FOUND";
const std::string BeanstalkReply::ok = "OK";

static const std::map<std::string, int> &BeanstalkReply::getReplyMap() {
	static std::map<std::string, int> *replyMap = NULL;

	if(replyMap == NULL) {
		replyMap = new std::map<std::string, int>();

		replyMap->insert(std::pair<std::string, int>(inserted, INSERTED));
		replyMap->insert(std::pair<std::string, int>(buried, BURIED));
		replyMap->insert(std::pair<std::string, int>(rusing, USING));
		replyMap->insert(std::pair<std::string, int>(reserved,RESERVED));
		replyMap->insert(std::pair<std::string, int>(watching, WATCHING));
		replyMap->insert(std::pair<std::string, int>(kicked, KICKED));
		replyMap->insert(std::pair<std::string, int>(found, FOUND));
		replyMap->insert(std::pair<std::string, int>(ok, OK));
	}

	return *replyMap;
}

int BeanstalkReply::getResponseType(const std::string &response) {
    std::map<std::string, int> replyMap = getReplyMap();
    std::map<std::string, int>::iterator it = replyMap.find(response);

    if(it == replyMap.end()) return -1;
    return it->second;
}

}
}
}
