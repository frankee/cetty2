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

static const int INIT_BUFFER_SIZE = 1024 * 256;
const std::string BeanstalkCommand::terminal_ = "\r\n";

BeanstalkCommand::BeanstalkCommand(const std::string& name)
    :name_(name) {
        buffer_ = Unpooled::buffer(INIT_BUFFER_SIZE);
        buffer_->setIndex(12, 12);
}

BeanstalkCommand::BeanstalkCommand(const std::string& name, int initBufferSize)
    : name_(name_) {
        if (initBufferSize <= 0) {
            initBufferSize = INIT_BUFFER_SIZE;
            LOG_WARN << "initBufferSize " << initBufferSize
                << " is negative, using default.";
        }

        buffer_ = Unpooled::buffer(initBufferSize);
        buffer_->setIndex(12, 12);
}

BeanstalkCommand::~BeanstalkCommand() {
}

BeanstalkCommand& BeanstalkCommand::append(const char* param, int size) {
    if (NULL == param || size <= 0) {
        return *this;
    }

    buffer_->writeBytes(param, size);
    return *this;
}

BeanstalkCommand& BeanstalkCommand::append(const std::string& param) {
    if (param.empty()) {
        return *this;
    }

    buffer_->writeBytes(param);
    return *this;
}

}
}
}
