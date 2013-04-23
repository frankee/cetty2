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

#include <cetty/beanstalk/builder/BeanstalkClientBuilder.h>

#include <cetty/beanstalk/protocol/BeanstalkReply.h>
#include <cetty/beanstalk/protocol/BeanstalkCommandEncoder.h>
#include <cetty/beanstalk/protocol/BeanstalkReplyDecoder.h>
#include <cetty/channel/ChannelPipeline.h>

namespace cetty {
namespace beanstalk {
namespace builder {

using namespace cetty::beanstalk::protocol;

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

bool initializeChannel(ChannelPipeline& pipeline) {
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
}
