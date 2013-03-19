/*
 * BeanstalkClientBuilder.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/BeanstalkClientBuilder.h>

#include <cetty/beanstalk/protocol/BeanstalkReply.h>
#include <cetty/beanstalk/BeanstalkReplyDecoder.h>
#include <cetty/beanstalk/BeanstalkCommandEncoder.h>
#include <cetty/channel/ChannelPipeline.h>

namespace cetty {
namespace beanstalk {

BeanstalkClientBuilder::BeanstalkClientBuilder()
    : builder_() {
    init();
}

BeanstalkClientBuilder::BeanstalkClientBuilder(int threadCnt)
    : builder_(threadCnt) {
    init();
}

BeanstalkClientBuilder::BeanstalkClientBuilder(const EventLoopPtr& eventLoop)
    : builder_(eventLoop) {
    init();
}

BeanstalkClientBuilder::BeanstalkClientBuilder(const EventLoopPoolPtr& eventLoopPool)
    : builder_(eventLoopPool) {
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
	builder_.setClientInitializer(boost::bind(initializeChannel, _1));
}


}
}
