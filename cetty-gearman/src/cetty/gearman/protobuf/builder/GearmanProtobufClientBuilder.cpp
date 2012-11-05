
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
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/gearman/GearmanClientHandler.h>
#include <cetty/gearman/protocol/GearmanMessageDecoder.h>
#include <cetty/gearman/protocol/GearmanMessageEncoder.h>
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
    : ClientBuilderType() {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(int threadCnt)
    : ClientBuilderType(threadCnt) {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(
    const EventLoopPoolPtr& eventLoopPool)
    : ClientBuilderType(eventLoopPool) {
    init();
}

GearmanProtobufClientBuilder::GearmanProtobufClientBuilder(
    const EventLoopPtr& eventLoop)
    :  ClientBuilderType(eventLoop) {
    init();
}

GearmanProtobufClientBuilder::~GearmanProtobufClientBuilder() {

}

void GearmanProtobufClientBuilder::init() {
    pipeline = ChannelPipelines::pipeline();

    pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024,8, 4, 0, 4));
    pipeline->addLast("gearmanDecoder", new GearmanMessageDecoder());
    pipeline->addLast("gearmanEncoder", new GearmanMessageEncoder());
    pipeline->addLast("gearmanClient", new GearmanClientHandler());
    pipeline->addLast("gearmanFilter", new GearmanProtobufClientFilter());

    ClientBuilderType::setPipeline(pipeline);
}

}
}
}
}
