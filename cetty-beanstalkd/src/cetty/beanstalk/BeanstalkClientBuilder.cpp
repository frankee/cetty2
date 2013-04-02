/*
 * BeanstalkClientBuilder.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/BeanstalkClientBuilder.h>

#include <cetty/beanstalk/protocol/BeanstalkReply.h>
#include <cetty/beanstalk/BeanstalkCommandEncoder.h>
#include <cetty/beanstalk/BeanstalkReplyDecoder.h>
#include <cetty/channel/ChannelPipeline.h>

namespace cetty {
namespace beanstalk {

BeanstalkClientBuilder::BeanstalkClientBuilder()
    : builder() {
    init();
}

BeanstalkClientBuilder::BeanstalkClientBuilder(int threadCnt)
    : builder(threadCnt) {
    init();
}

BeanstalkClientBuilder::BeanstalkClientBuilder(const EventLoopPtr& eventLoop)
    : builder(eventLoop) {
    init();
}

BeanstalkClientBuilder::BeanstalkClientBuilder(const EventLoopPoolPtr& eventLoopPool)
    : builder(eventLoopPool) {
    init();
}

bool initializeChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<BeanstalkCommandEncoder::HandlerPtr>("beanstalkEncoder",
        BeanstalkCommandEncoder::HandlerPtr(new BeanstalkCommandEncoder));

    pipeline.addLast<BeanstalkReplyDecoder::HandlerPtr>("beanstalkDecoder",
        BeanstalkReplyDecoder::HandlerPtr(new BeanstalkReplyDecoder));

    return true;
}

void BeanstalkClientBuilder::init() {
	builder.setClientInitializer(boost::bind(initializeChannel, _1));
}


}
}
