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

#include <cetty/craft/builder/CraftServerBuilder.h>

#include <cetty/channel/ChannelPipeline.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/LengthFieldPrepender.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpServerCodec.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>
#include <cetty/craft/http/HttpServiceFilter.h>
#include <cetty/craft/http/ServiceRequestMapping.h>
#include <cetty/craft/http/ServiceResponseMapping.h>

namespace cetty {
namespace craft {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::handler::codec::http;
using namespace cetty::service;
using namespace cetty::config;
using namespace cetty::protobuf::service::handler;
using namespace cetty::protobuf::service::builder;
using namespace cetty::craft::http;

static const std::string PROTOBUF_SERVICE_HTTP("http");

CraftServerBuilder::CraftServerBuilder()
    : builder_() {
    init();
}

CraftServerBuilder::CraftServerBuilder(int parentThreadCnt, int childThreadCnt)
    : builder_(parentThreadCnt, childThreadCnt) {
    init();
}

CraftServerBuilder::~CraftServerBuilder() {
}

CraftServerBuilder& CraftServerBuilder::registerService(const ProtobufServicePtr& service) {
    builder_.registerService(service);

    //initialize the mapping.
    ServiceRequestMapping::instance();
    ServiceResponseMapping::instance();

    return *this;
}

ChannelPtr CraftServerBuilder::buildHttp(int port) {
    return builder_.serverBuilder().build(PROTOBUF_SERVICE_HTTP, port);
}

bool CraftServerBuilder::initializeChildChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    //if (ssl) {
    //}

    pipeline.addLast<HttpServerCodec::HandlerPtr>("httpCodec",
            HttpServerCodec::HandlerPtr(new HttpServerCodec));

    // Remove the following line if you don't want automatic content compression.
    //pipeline.addLast("deflater", new HttpContentCompressor());

    pipeline.addLast<HttpServiceFilter::HandlerPtr>("protobufFilter",
            HttpServiceFilter::HandlerPtr(
                new HttpServiceFilter()));

    pipeline.addLast<ProtobufServiceMessageHandler::HandlerPtr>("messageHandler",
            ProtobufServiceMessageHandler::HandlerPtr(new ProtobufServiceMessageHandler));

    return true;
}

void CraftServerBuilder::init() {
    builder_.serverBuilder().registerPrototype(PROTOBUF_SERVICE_HTTP,
                                            boost::bind(&CraftServerBuilder::initializeChildChannel,
                                                    this,
                                                    _1));
}

}
}
}
