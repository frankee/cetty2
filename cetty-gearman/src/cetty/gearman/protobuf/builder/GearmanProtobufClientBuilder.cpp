
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
    : builder_() {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(int threadCnt)
    : builder_(threadCnt) {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(
    const EventLoopPoolPtr& eventLoopPool)
    : builder_(eventLoopPool) {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(
    const EventLoopPtr& eventLoop)
    :  builder_(eventLoop) {
    init();
}

bool initializeChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<LengthFieldBasedFrameDecoder::HandlerPtr>("frameDecoder",
        LengthFieldBasedFrameDecoder::HandlerPtr(
        new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 8, 4, 0, 4)));

    pipeline.addLast<GearmanMessageCodec::HandlerPtr>("gearmanCodec",
        GearmanMessageCodec::HandlerPtr(new GearmanMessageCodec));

    pipeline.addLast<GearmanClientHandler::HandlerPtr>("gearmanClient",
        GearmanClientHandler::HandlerPtr(new GearmanClientHandler));

    pipeline.addLast<GearmanProtobufClientFilter::HandlerPtr>("gearmanFilter",
        GearmanProtobufClientFilter::HandlerPtr(new GearmanProtobufClientFilter));

    return true;
}

void GearmanProtobufClientBuilder::init() {
    builder_.setClientInitializer(boost::bind(&initializeChannel, _1));
}

}
}
}
}
