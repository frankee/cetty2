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

#include <cetty/shuttle/protobuf/ProtobufProxyHandler.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>

#include <cetty/shuttle/ShuttleBackend.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessage.h>

#include <cetty/protobuf/service/service.pb.h>
#include <cetty/service/ClientService.h>

namespace cetty {
namespace shuttle {
namespace protobuf {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::protobuf::service;

void ProtobufProxyHandler::messageUpdated(ChannelHandlerContext& ctx) {
    InboundQueue& inboundQueue =
        context_->inboundContainer()->getMessages();

    while (!inboundQueue.empty()) {
        ProtobufProxyMessagePtr& msg = inboundQueue.front();

        if (!msg) {
            break;
        }

        messageReceived(ctx, msg);
        inboundQueue.pop_front();
    }
}

void ProtobufProxyHandler::messageReceived(ChannelHandlerContext& ctx,
        const ProtobufProxyMessagePtr& msg) {
    const ServiceMessage& rpc = msg->message();
    std::string method(rpc.service());
    method.append(1, '.');
    method.append(rpc.method());

    const ChannelPtr& ch = ShuttleBackend::getChannel(method);

    if (ch) {
        LOG_INFO << "proxy the method " << method
                 << " to backend " << ch->remoteAddress().toString();

        callMethod<ProtobufProxyMessagePtr, ProtobufProxyMessagePtr>(ch,
                msg,
                new ProxyFuture(boost::bind(&ProtobufProxyHandler::onResponse,
                                            this,
                                            _1,
                                            _2)));
    }
    else {
        LOG_WARN << "failed to found the channel for " << method;
    }
}

void ProtobufProxyHandler::onResponse(const ProxyFuture& future,
                                      const ProtobufProxyMessagePtr& response) {
    context_->outboundTransfer()->write(response, context_->newVoidFuture());
}

}
}
}
