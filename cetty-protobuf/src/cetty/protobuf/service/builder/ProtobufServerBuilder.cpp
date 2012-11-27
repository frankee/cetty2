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

#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/LengthFieldPrepender.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpRequestDecoder.h>
#include <cetty/handler/codec/http/HttpResponseEncoder.h>
#include <cetty/handler/codec/http/HttpChunkAggregator.h>
#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>

#include <cetty/channel/CombinedChannelMessageHandler.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::handler::codec::http;
using namespace cetty::service;
using namespace cetty::config;
using namespace cetty::protobuf::service::handler;

static const std::string PROTOBUF_SERVICE_RPC("rpc");

ProtobufServerBuilder::ProtobufServerBuilder()
    : ServerBuilder() {
    init();
}

ProtobufServerBuilder::ProtobufServerBuilder(int parentThreadCnt, int childThreadCnt)
    : ServerBuilder(parentThreadCnt, childThreadCnt) {
    init();
}

ProtobufServerBuilder::~ProtobufServerBuilder() {
}

ProtobufServerBuilder& ProtobufServerBuilder::registerService(
    const ProtobufServicePtr& service) {
    ProtobufServiceRegister::instance().registerService(service);
    return *this;
}

ChannelPtr ProtobufServerBuilder::buildRpc(int port) {
    return build(PROTOBUF_SERVICE_RPC, port);
}

void ProtobufServerBuilder::init() {
    registerPipeline(PROTOBUF_SERVICE_RPC, createProtobufServicePipeline());
}

class ProtobufServiceCodec
    : public CombinedChannelMessageHandler<ChannelBufferPtr,
    ProtobufServiceMessagePtr,
    ProtobufServiceMessagePtr,
    ChannelBufferPtr,
    ChannelOutboundBufferHandlerContext> {
public:
    ProtobufServiceCodec()
        : CombinedChannelMessageHandler<ChannelBufferPtr,
    ProtobufServiceMessagePtr,
    ProtobufServiceMessagePtr,
    ChannelBufferPtr,
    ChannelOutboundBufferHandlerContext> (new ProtobufServiceMessageDecoder,
        new ProtobufServiceMessageEncoder) {}

    virtual ~ProtobufServiceCodec() {}

    virtual ChannelHandlerPtr clone() {
        return new ProtobufServiceCodec;
    }

    virtual std::string toString() const {
        return "ProtobufServiceCodec";
    }
};

ChannelPipelinePtr ProtobufServerBuilder::createProtobufServicePipeline() {
    ChannelPipelinePtr pipeline = ChannelPipelines::pipeline();

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

    pipeline->addLast("protoCodec", new ProtobufServiceCodec);
    //pipeline->addLast("protobufDecoder", new ProtobufServiceMessageDecoder());
    //pipeline->addLast("protobufEncoder", new ProtobufServiceMessageEncoder());

    pipeline->addLast("messageHandler", new ProtobufServiceMessageHandler());

    return pipeline;
}

}
}
}
}
