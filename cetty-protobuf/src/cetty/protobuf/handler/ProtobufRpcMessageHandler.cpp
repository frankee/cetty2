/*
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

#include <cetty/protobuf/handler/ProtobufRpcMessageHandler.h>

#include <boost/thread/locks.hpp>
#include <boost/bind.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelStateEvent.h>
#include <cetty/channel/ChannelHandlerContext.h>

#include <cetty/protobuf/ProtobufService.h>
#include <cetty/protobuf/ProtobufServiceFuture.h>
#include <cetty/protobuf/ProtobufServiceRegister.h>
#include <cetty/protobuf/handler/ProtobufRpcMessage.h>
#include <cetty/protobuf/proto/rpc.pb.h>

namespace cetty {
namespace protobuf {
namespace handler {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::protobuf;
using namespace cetty::protobuf::proto;
using namespace google::protobuf;

ProtobufRpcMessageHandler::ProtobufRpcMessageHandler() {
}

ProtobufRpcMessageHandler::~ProtobufRpcMessageHandler() {
}

void ProtobufRpcMessageHandler::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    ProtobufRpcMessagePtr msg = e.getMessage().smartPointer<ProtobufRpcMessage>();
    const RpcMessage& rpc = msg->getRpcMessage();

    if (NULL == msg) {
        ctx.sendUpstream(e);
    }
    else if (rpc.type() == cetty::protobuf::proto::REQUEST) {
        ProtobufServicePtr& service = 
            ProtobufServiceRegister::instance().getService(rpc.service());

        const google::protobuf::MethodDescriptor* method =
            ProtobufServiceRegister::instance().getMethodDescriptor(service,
            rpc.method());

        boost::int64_t id = rpc.id();

        if (method) {
            ProtobufServiceFuturePtr future(new ProtobufServiceFuture);
            future->addListenser(boost::bind(&ProtobufRpcMessageHandler::doneCallback,
                                             this,
                                             _1,
                                             _2,
                                             id));

            service->CallMethod(method, msg->getPayload(), future);
        }
        else {
            // return error message
            printf("has no such service or method.\n");
        }
    }
    else if (rpc.type() == cetty::protobuf::proto::ERROR) {
    }
    else {
        ctx.sendUpstream(e);
    }
}

void deleteMessage(const ChannelFuturePtr& future, proto::RpcMessage* message) {
    if (message) {
        delete message;
    }
}

void ProtobufRpcMessageHandler::doneCallback(google::protobuf::Message* response,
        boost::int64_t id) {
    proto::RpcMessage* message = new proto::RpcMessage;
    message->set_type(proto::RESPONSE);
    message->set_id(id);

    ChannelFuturePtr future = channel->write(ChannelMessage((MessageLite*)(message)));
    future->setListener(boost::bind(deleteMessage, _1, message));

    delete response;
}

cetty::channel::ChannelHandlerPtr ProtobufRpcMessageHandler::clone() {
    return ChannelHandlerPtr(new ProtobufRpcMessageHandler());
}

std::string ProtobufRpcMessageHandler::toString() const {
    return "ProtobufRpcMessageHandler";
}

}
}
}