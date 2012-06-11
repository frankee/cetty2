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

#include <cetty/protobuf/handler/ProtobufRpcMessageHandler.h>

#include <boost/thread/locks.hpp>
#include <boost/bind.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/handler/rpc/protobuf/rpc.pb.h"

namespace cetty { namespace protobuf { namespace handler {

using namespace cetty::channel;
using namespace google::protobuf;

ProtobufRpcMessageHandler::ProtobufRpcMessageHandler()
    : channel(NULL), id(NULL) {
}

ProtobufRpcMessageHandler::~ProtobufRpcMessageHandler() {
}

void ProtobufRpcMessageHandler::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    ProtobufRpcMessage* message = e.getMessage()<proto::RpcMessage, google::protobuf::MessageLite>();

    if (NULL == message) {
        ctx.sendUpstream(e);
    }
    else if (message->rpc->type() == proto::RESPONSE) {
        boost::int64_t id = message->id();
        assert(message->has_response());

        OutstandingCall out = { NULL, NULL, NULL, NULL};
        {
            std::map<boost::int64_t, OutstandingCall>::iterator it = outstandings.find(id);
            if (it != outstandings.end()) {
                out = it->second;
                outstandings.erase(it);
            }
        }

        if (out.response) {
            out.response->CopyFrom(message->payload);
            if (out.done) {
                out.done->Run();
            }
            delete out.message;
        }
    }
    else if (message->rpc->type() == proto::REQUEST) {
        ProtobufServicePtr& service = serviceRegister->getService(message->rpc->service());
        const google::protobuf::MethodDescriptor* method =
            serviceRegister->getMethodDescriptor(service, message->method());

        boost::int64_t id = message->rpc->id();

        if (method) {
            google::protobuf::Message* response = service->GetResponsePrototype(method).New();
            service->CallMethod(method, NULL, message->payload, response,
                boost::bind(&ProtobufRpcMessageHandler::doneCallback, this, response, id));
        }
        else {
            // return error message
            printf("has no such service or method.\n");
        }
    }
    else if (message->type() == proto::ERROR) {
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

void ProtobufRpcMessageHandler::doneCallback(google::protobuf::Message* response, boost::int64_t id) {
    proto::RpcMessage* message = new proto::RpcMessage;
    message->set_type(proto::RESPONSE);
    message->set_id(id);

    ChannelFuturePtr future = channel->write(ChannelMessage((MessageLite*)(message)));
    future->setListener(boost::bind(deleteMessage, _1, message));

    delete response;
}

void ProtobufRpcMessageHandler::channelConnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    this->channel = e.getChannel();
}

void ProtobufRpcMessageHandler::channelDisconnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    this->channel = NULL;
}

cetty::channel::ChannelHandlerPtr ProtobufRpcMessageHandler::clone() {
    ProtobufRpcMessageHandler* handler = new ProtobufRpcMessageHandler();
    return ChannelHandlerPtr(dynamic_cast<ChannelHandler*>(handler));
}

void ProtobufRpcMessageHandler::exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
    ctx.getChannel().close();
}

void ProtobufRpcMessageHandler::writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
    
    OutstandingCall call;

    if (call.rpc && call.rpc->type() == proto::REQUEST) {
        if (!call.rpc->has_id()) {
            message->set_id(++this->id);
        }

        outstandings[id] = call;
        
        ctx.sendDownstream();
    }
}

}}}