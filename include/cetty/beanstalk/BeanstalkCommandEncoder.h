/*
 * beanstalkCommandEncoder.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef BEANSTALKCOMMANDENCODER_H_
#define BEANSTALKCOMMANDENCODER_H_

#include <cetty/handler/codec/MessageToBufferEncoder.h>
#include <cetty/beanstalk/protocol/BeanstalkCommand.h>

namespace cetty {
namespace beanstalk {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::beanstalk::protocol;

class BeanstalkCommandEncoder : private boost::noncopyable {
public:
    typedef MessageToBufferEncoder<BeanstalkCommandEncoder, BeanstalkCommandPtr> Encoder;
    typedef Encoder::Context Context;
    typedef Encoder::Handler Handler;
    typedef Encoder::HandlerPtr HandlerPtr;

public:
    BeanstalkCommandEncoder() {
        encoder.setEncoder(boost::bind(&BeanstalkCommandEncoder::encode,
                                        this,
                                        _1,
                                        _2,
                                        _3));
    }

    ~BeanstalkCommandEncoder() {}

    void registerTo(Context& ctx) {
        encoder.registerTo(ctx);
    }

private:
    ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                            const BeanstalkCommandPtr& msg,
                            const ChannelBufferPtr& out) {
        return msg->getBuffer();
    }

private:
    Encoder encoder;
};

}
}

#endif /* BEANSTALKCOMMANDENCODER_H_ */
