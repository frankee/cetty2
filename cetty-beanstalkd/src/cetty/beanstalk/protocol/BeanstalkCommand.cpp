/*
 * BeanstalkCommand.cpp
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#include "cetty/beanstalk/protocol/BeanstalkCommand.h"

namespace cetty {
namespace beanstalk {
namespace protocol {

const std::string BeanstalkCommand::terminal = "\r\n";

BeanstalkCommand& BeanstalkCommand::append(const char* param, int size) {
    if (NULL == param || size <= 0) {
        return *this;
    }

    buffer->writeBytes(param, size);
    return *this;
}


BeanstalkCommand& BeanstalkCommand::append(const std::string& param) {
    if (param.empty()) {
        return *this;
    }

    buffer->writeBytes(param);
    return *this;
}


}
}
}
