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

#include <cetty/shuttle/ShuttleBuilder.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/LengthFieldPrepender.h>
#include <cetty/service/builder/ServerBuilder.h>
#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>
#include <cetty/shuttle/ShuttleConfig.cnf.h>
#include <cetty/shuttle/ShuttleBackend.h>
#include <cetty/shuttle/protobuf/ProtobufProxyCodec.h>
#include <cetty/shuttle/protobuf/ProtobufProxyHandler.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessage.h>

namespace cetty {
namespace shuttle {

using namespace cetty::channel;
using namespace cetty::config;
using namespace cetty::handler::codec;
using namespace cetty::service;
using namespace cetty::service::builder;
using namespace cetty::protobuf::service;
using namespace cetty::shuttle::protobuf;

ShuttleBuilder::ShuttleBuilder()
    : builder_() {
    init();
    ConfigCenter::instance().configure(&config_);

    if (!config_.backends.empty()) {
        ShuttleBackend::configure(config_.backends, builder_.childPool());
    }
    else {
        LOG_WARN << "has no backends set";
    }
}

ShuttleBuilder::~ShuttleBuilder() {

}

void ShuttleBuilder::init() {
    builder_.registerPrototype("shuttle",
                               boost::bind(&ShuttleBuilder::initializeChannel,
                                           this,
                                           _1));
}

bool ShuttleBuilder::initializeChannel(ChannelPipeline& pipeline) {
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

    pipeline.addLast<ProtobufProxyCodec::Handler>(
        "protobufCodec",
        ProtobufProxyCodec::HandlerPtr(new ProtobufProxyCodec));

    pipeline.addLast<ProtobufProxyHandler>(
        "messageHandler",
        ProtobufProxyHandler::HandlerPtr(new ProtobufProxyHandler));

    return true;
}

void ShuttleBuilder::waitingForExit() {
    builder_.waitingForExit();
}

ShuttleBuilder& ShuttleBuilder::build() {
    builder_.buildAll();
    return *this;
}

}
}
