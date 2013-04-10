#if !defined(CETTY_BEANSTALK_PROTOCOL_BEANSTALKCOMMAND_H)
#define CETTY_BEANSTALK_PROTOCOL_BEANSTALKCOMMAND_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 *      Author: chenhl
 */

#include <string>
#include <cstdlib>

#include <boost/lexical_cast.hpp>
#include <boost/intrusive_ptr.hpp>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/Unpooled.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/beanstalk/protocol/BeanstalkCommandPtr.h>

namespace cetty {
namespace beanstalk {
namespace protocol {

using namespace cetty::buffer;
using namespace cetty::util;

class BeanstalkCommand : public ReferenceCounter<BeanstalkCommand, int> {
public:
	BeanstalkCommand(const std::string& name);
    BeanstalkCommand(const std::string& name, int initBufferSize);

    ~BeanstalkCommand();

    const std::string& name() const;

    BeanstalkCommand& append(const char* param, int size);
    BeanstalkCommand& append(const std::string& param);

    BeanstalkCommand& operator<< (const std::string& param);
    BeanstalkCommand& operator<< (const StringPiece& param);

    template <typename T>
    BeanstalkCommand& operator<<(T const& datum);

    void done();

    const ChannelBufferPtr& buffer() const;

private:
    static const std::string terminal_;

private:
    std::string name_;
    ChannelBufferPtr buffer_;
};

inline
const std::string& BeanstalkCommand::name() const {
    return this->name_;
}

inline
BeanstalkCommand& BeanstalkCommand::operator<<(const std::string& param) {
    return append(param);
}

inline
BeanstalkCommand& BeanstalkCommand::operator<<(const StringPiece& param) {
    return append(param.data(), param.length());
}

template <typename T> inline
BeanstalkCommand& BeanstalkCommand::operator<<(T const& datum) {
    try {
        return append(boost::lexical_cast<std::string>(datum));
    }
    catch (const std::exception& e) {
        LOG_ERROR << "append data has exception " << e.what();
        return *this;
    }
}

inline
void BeanstalkCommand::done() {
    append(terminal_);
}

inline
const ChannelBufferPtr& BeanstalkCommand::buffer() const {
    return buffer_;
}

}
}
}

#endif //#if !defined(CETTY_BEANSTALK_PROTOCOL_BEANSTALKCOMMAND_H)

// Local Variables:
// mode: c++
// End:
