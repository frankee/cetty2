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

#include <cetty/protobuf/service/builder/ProtobufServerBuilder.h>

#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/LengthFieldPrepender.h>
#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageCodec.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::service;
using namespace cetty::config;
using namespace cetty::protobuf::service::handler;

static const std::string PROTOBUF_SERVICE_RPC("rpc");

ProtobufServerBuilder::ProtobufServerBuilder()
    : builder_() {
    init();
}

ProtobufServerBuilder::ProtobufServerBuilder(int parentThreadCnt,
        int childThreadCnt)
    : builder_(parentThreadCnt, childThreadCnt) {
    init();
}

ProtobufServerBuilder::~ProtobufServerBuilder() {
}

ProtobufServerBuilder& ProtobufServerBuilder::registerService(
    const ProtobufServicePtr& service) {
    ProtobufServiceRegister::instance().registerService(service);
    return *this;
}

bool ProtobufServerBuilder::initializeChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<LengthFieldBasedFrameDecoder::Handler>(
        "frameDecoder",
        LengthFieldBasedFrameDecoder::HandlerPtr(new LengthFieldBasedFrameDecoder(
                    16 * 1024 * 1024,
                    0,
                    4,
                    0,
                    4,
                    4,
                    ProtobufUtil::adler32Checksum)));

    pipeline.addLast<LengthFieldPrepender::Handler>(
        "frameEncoder",
        LengthFieldPrepender::HandlerPtr(new LengthFieldPrepender(
                4,
                4,
                ProtobufUtil::adler32Checksum)));

    pipeline.addLast<ProtobufServiceMessageCodec::Handler>(
        "protobufCodec",
        ProtobufServiceMessageCodec::HandlerPtr(new ProtobufServiceMessageCodec));

    pipeline.addLast<ProtobufServiceMessageHandler>(
        "messageHandler",
        ProtobufServiceMessageHandler::HandlerPtr(new ProtobufServiceMessageHandler));

    return true;
}

ChannelPtr ProtobufServerBuilder::buildRpc(int port) {
    return builder_.build(PROTOBUF_SERVICE_RPC, port);
}

void ProtobufServerBuilder::init() {
    builder_.registerServerPrototype(PROTOBUF_SERVICE_RPC,
                   boost::bind(&ProtobufServerBuilder::initializeChannel,
                               this,
                               _1));
}

}
}
}
}
