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
#include <map>

namespace cetty {
namespace beanstalk {
namespace protocol {

using namespace cetty::util;

class BeanstalkReply : public ReferenceCounter<BeanstalkReply> {
public:
	BeanstalkReply():id (-1), count(-1){}

	void setId(int id) { this->id = id; }
	int getId() { return id; }

	void setCount(int count) { this->count = count; }
	int getCount() { return count; }

	void setResponse(const std::string &response) {
		this->response = response;
	}

	const std::string &getResponse() { return response; }

	void setValue(const std::string &value) { this->value = value; }
    const std::string &getValue() { return value; }

    int getResponseType(const std::string &response);

public:

    enum {INSERTED, BURIED, USING, RESERVED, WATCHING, KICKED, FOUND, OK};

private:
    static const std::map<std::string, int> &getReplyMap();

private:
    static const std::string inserted;
    static const std::string buried;
    static const std::string rusing;
    static const std::string reserved;
    static const std::string watching;
    static const std::string kicked;
    static const std::string found;
    static const std::string ok;

    int id;
    int count;
    std::string response;
    std::string value;
};

typedef boost::intrusive_ptr<BeanstalkReply> BeanstalkReplyPtr;

}
}
}


#endif /* BEANTALKREPLY_H_ */
