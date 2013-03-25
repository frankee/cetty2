
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

#include <cetty/gearman/protobuf/builder/GearmanProtobufClientBuilder.h>

#include <cetty/channel/ChannelPipeline.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/gearman/GearmanClientHandler.h>
#include <cetty/gearman/protocol/GearmanMessageCodec.h>
#include <cetty/gearman/protobuf/GearmanProtobufClientFilter.h>

namespace cetty {
namespace gearman {
namespace protobuf {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::gearman::protocol;
using namespace cetty::protobuf::service;

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder()
    : init_(false),
      background_(false),
      builder_() {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(int threadCnt)
    : init_(false),
      background_(false),
      builder_(threadCnt) {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(
    const EventLoopPoolPtr& eventLoopPool)
    : init_(false),
      background_(false),
      builder_(eventLoopPool) {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(
    const EventLoopPtr& eventLoop)
    :  init_(false),
       background_(false),
       builder_(eventLoop) {
    init();
}

bool GearmanProtobufClientBuilder::initializeChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<LengthFieldBasedFrameDecoder::HandlerPtr>("frameDecoder",
            LengthFieldBasedFrameDecoder::HandlerPtr(
                new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 8, 4, 0, 4)));

    pipeline.addLast<GearmanMessageCodec::HandlerPtr>("gearmanCodec",
            GearmanMessageCodec::HandlerPtr(new GearmanMessageCodec));

    pipeline.addLast<GearmanClientHandler::HandlerPtr>("gearmanClient",
            GearmanClientHandler::HandlerPtr(new GearmanClientHandler));

    GearmanProtobufClientFilter::HandlerPtr filter(
        new GearmanProtobufClientFilter);

    filter->setBackground(background_);
    filter->setUniqueKey(uniqueKey_);

    pipeline.addLast<GearmanProtobufClientFilter::HandlerPtr>("gearmanFilter",
            filter);

    return true;
}

void GearmanProtobufClientBuilder::init() {
    if (!init_) {
        builder_.setClientInitializer(boost::bind(
                                          &GearmanProtobufClientBuilder::initializeChannel,
                                          this,
                                          _1));

        init_ = true;
    }
}

cetty::channel::ChannelPtr GearmanProtobufClientBuilder::build() {
    return builder_.build();
}

}
}
}
}
