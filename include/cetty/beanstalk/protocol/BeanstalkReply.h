/*
 * BeantalkReply.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef BEANTALKREPLY_H_
#define BEANTALKREPLY_H_

#include <cetty/Types.h>
#include <cetty/util/Enum.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>

#include <boost/variant.hpp>
#include <vector>

namespace cetty {
namespace beanstalk {
namespace protocol {

using namespace cetty::util;

class BeanstalkReply : public ReferenceCounter<BeanstalkReply> {
public:
	BeanstalkReply();

	void setResponse(const std::string &response) {
		this->response = response;
	}

	const std::string &getResponse() { return response; }
    std::vector<std::string> &getValue() { return value; }

    int getResponseType(const std::string &response);
    static const std::map<std::string, int> &getReplyMap();

private:
    static const int INSERTED;
    static const int BURIED;
    static const int USING;
    static const int RESERVED;
    static const int WATCHING;
    static const int KICKED;
    static const int FOUND;
    static const int OK;

    static const std::string inserted;
    static const std::string buried;
    static const std::string rusing;
    static const std::string reserved;
    static const std::string watching;
    static const std::string kicked;
    static const std::string found;
    static const std::string ok;

    std::string response;
    std::vector<std::string> value;
};

typedef boost::intrusive_ptr<BeanstalkReply> BeanstalkReplyPtr;

}
}
}


#endif /* BEANTALKREPLY_H_ */
