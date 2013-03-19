/*
 * BeanstalkCommand.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef BEANSTALKCOMMAND_H_
#define BEANSTALKCOMMAND_H_

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/Unpooled.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>

#include <boost/lexical_cast.hpp>
#include <boost/intrusive_ptr.hpp>

#include <cstdlib>
#include <string>

namespace cetty {
namespace beanstalk {
namespace protocol {

#define DEFAULT_PRIORITY 1024
#define DEFAULT_TTR 60

using namespace cetty::buffer;
using namespace cetty::util;

class BeanstalkCommand : public cetty::util::ReferenceCounter<BeanstalkCommand, int> {
public:
	BeanstalkCommand(const std::string& name)
        :name(name) {
        buffer = Unpooled::buffer(512*1024);
        buffer->setIndex(12, 12);
    }

    ~BeanstalkCommand() {}

    const std::string& getCommandName() const {
        return this->name;
    }

    BeanstalkCommand& append(const char* param, int size);
    BeanstalkCommand& append(const std::string& param);

    BeanstalkCommand& operator<< (const std::string& param) {
        return append(param);
    }

    BeanstalkCommand& operator<< (const StringPiece& param) {
        return append(param.data(), param.length());
    }

    template <typename T>
    BeanstalkCommand& operator<<(T const& datum) {
        return append(boost::lexical_cast<std::string>(datum));
    }


    void done() { append(terminal); }

    const ChannelBufferPtr& getBuffer() const { return buffer; }

private:
    static const std::string terminal;
    std::string name;
    ChannelBufferPtr buffer;
};

typedef boost::intrusive_ptr<BeanstalkCommand> BeanstalkCommandPtr;

}
}
}

#endif /* BEANSTALKCOMMAND_H_ */
