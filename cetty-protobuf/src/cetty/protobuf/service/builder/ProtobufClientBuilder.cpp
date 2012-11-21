
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

#include <cetty/protobuf/service/builder/ProtobufClientBuilder.h>

#include <cetty/channel/ChannelPipelines.h>
#include <cetty/handler/codec/LengthFieldPrepender.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>

#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>
#include <cetty/protobuf/service/handler/ProtobufServiceRequestHandler.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::protobuf::service::handler;

ProtobufClientBuilder::ProtobufClientBuilder()
    : ClientBuilderType() {
    init();
}

ProtobufClientBuilder::ProtobufClientBuilder(int threadCnt)
    : ClientBuilderType(threadCnt) {
    init();
}

ProtobufClientBuilder::ProtobufClientBuilder(const EventLoopPtr& eventLoop)
    : ClientBuilderType(eventLoop) {
    init();
}

ProtobufClientBuilder::ProtobufClientBuilder(const EventLoopPoolPtr& eventLoopPool)
    : ClientBuilderType(eventLoopPool) {
    init();
}

void ProtobufClientBuilder::init() {
    pipeline = ChannelPipelines::pipeline();

    pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(
                          16 * 1024 * 1024,
                          0,
                          4,
                          0,
                          4,
                          4,
                          ProtobufUtil::adler32Checksum));

    pipeline->addLast("frameEncoder", new LengthFieldPrepender(
                          4,
                          4,
                          ProtobufUtil::adler32Checksum));

    pipeline->addLast("protobufDecoder", new ProtobufServiceMessageDecoder());
    pipeline->addLast("protobufEncoder", new ProtobufServiceMessageEncoder());

    //pipeline->addLast("messageHandler", new ProtobufServiceMessageHandler());

    ClientBuilderType::setPipeline(pipeline);
}

}
}
}
}
