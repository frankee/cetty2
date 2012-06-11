/**
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include "cetty/handler/rpc/protobuf/ProtobufRpcPipelineFactory.h"

#include "boost/assert.hpp"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h"
#include "cetty/handler/codec/frame/LengthFieldPrepender.h"
#include "cetty/handler/codec/protobuf/ProtobufDecoder.h"
#include "cetty/handler/codec/protobuf/ProtobufEncoder.h"
#include "cetty/handler/rpc/protobuf/rpc.pb.h"
#include "cetty/handler/rpc/protobuf/ProtobufRpcMessageHandler.h"


namespace cetty { namespace handler { namespace rpc { namespace protobuf {

using namespace cetty::channel;
using namespace cetty::handler::codec::frame;
using namespace cetty::handler::codec::protobuf;

ChannelPipeline* ProtobufRpcPipelineFactory::getPipeline() {
    // Create a default pipeline implementation.
    ChannelPipeline* pipeline = Channels::pipeline();
    BOOST_ASSERT(pipeline);

    pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 0, 4, 0, 4));
    pipeline->addLast("frameEncoder", new LengthFieldPrepender(4));

    pipeline->addLast("protobufDecoder", new ProtobufDecoder(&(proto::RpcMessage::default_instance())));
    pipeline->addLast("protobufEncoder", new ProtobufEncoder());

    ProtobufRpcMessageHandlerPtr handler;
    if (this->messageHandler) {
        handler = boost::dynamic_pointer_cast<ProtobufRpcMessageHandler>(this->messageHandler->clone());
    }
    else {
        handler = ProtobufRpcMessageHandlerPtr(new ProtobufRpcMessageHandler());
    }
    handler->setServices(&(this->servicesRegister));

    google::protobuf::RpcChannel* rpcChannel =
        dynamic_cast<google::protobuf::RpcChannel*>(handler.get());
    pipeline->setAttachment(rpcChannel);

    pipeline->addLast("handler", handler);

    return pipeline;
}

void ProtobufRpcPipelineFactory::setMessageHandler(const ProtobufRpcMessageHandlerPtr& handler) {
    this->messageHandler.reset();
    this->messageHandler = handler;
}

}}}}