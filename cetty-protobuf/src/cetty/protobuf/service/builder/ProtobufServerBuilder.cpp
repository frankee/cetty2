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

#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/frame/LengthFieldPrepender.h>
#include <cetty/handler/codec/http/HttpRequestDecoder.h>
#include <cetty/handler/codec/http/HttpResponseEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>
#include <cetty/protobuf/service/http/ProtobufHttpMessageFilter.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec::frame;
using namespace cetty::handler::codec::http;
using namespace cetty::service;
using namespace cetty::config;
using namespace cetty::protobuf::service::handler;
using namespace cetty::protobuf::service::http;

static const std::string PROTOBUF_SERVICE_HTTP("http");
static const std::string PROTOBUF_SERVICE_RPC("rpc");

ProtobufServerBuilder::ProtobufServerBuilder(int threadCnt)
    : ServerBuilder(threadCnt) {
    init();
}

ProtobufServerBuilder::ProtobufServerBuilder(const ConfigCenter& conf)
    : ServerBuilder(conf), confCenter(&conf) {
    init();
}

ProtobufServerBuilder::~ProtobufServerBuilder() {
}

ProtobufServerBuilder& ProtobufServerBuilder::registerService(
    const ProtobufServicePtr& service) {
    ProtobufServiceRegister::instance().registerService(service);
    return *this;
}

ChannelPtr ProtobufServerBuilder::buildHttp(int port) {
    return build(PROTOBUF_SERVICE_HTTP, port);
}

ChannelPtr ProtobufServerBuilder::buildRpc(int port) {
    return build(PROTOBUF_SERVICE_RPC, port);
}

void ProtobufServerBuilder::init() {
    if (confCenter) {
        registerPipeline(PROTOBUF_SERVICE_HTTP, createProtobufHttpServicePipeline(*confCenter));
    }
    else {
        printf("has not set the configCenter, so can't inti the http service.\n");
    }

    registerPipeline(PROTOBUF_SERVICE_RPC, createProtobufServicePipeline());
}

ChannelPipelinePtr ProtobufServerBuilder::createProtobufServicePipeline() {
    ChannelPipelinePtr pipeline = Channels::pipeline();

    pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 0, 4, 0, 4));
    pipeline->addLast("frameEncoder", new LengthFieldPrepender(4));

    pipeline->addLast("protobufDecoder", new ProtobufServiceMessageDecoder());
    pipeline->addLast("protobufEncoder", new ProtobufServiceMessageEncoder());

    pipeline->addLast("messageHandler", new ProtobufServiceMessageHandler());

    return pipeline;
}

ChannelPipelinePtr ProtobufServerBuilder::createProtobufHttpServicePipeline(
    const ConfigCenter& confCenter) {
    ChannelPipelinePtr pipeline = Channels::pipeline();

    //if (ssl) {
    //}

    pipeline->addLast("decoder", new HttpRequestDecoder());

    // Uncomment the following line if you don't want to handle HttpChunks.
    //pipeline.addLast("aggregator", new HttpChunkAggregator(1048576));

    pipeline->addLast("encoder", new HttpResponseEncoder());

    // Remove the following line if you don't want automatic content compression.
    //pipeline.addLast("deflater", new HttpContentCompressor());

    pipeline->addLast("protobufFilter", new ProtobufHttpMessageFilter(confCenter));

    pipeline->addLast("messageHandler", new ProtobufServiceMessageHandler());

    return pipeline;
}

}
}
}
}
