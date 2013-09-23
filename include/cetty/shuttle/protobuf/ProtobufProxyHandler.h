#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFHANDLER_H)
#define CETTY_SHUTTLE_PROTOBUF_PROTOBUFHANDLER_H

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

#include <deque>
#include <cetty/Types.h>
#include <cetty/channel/ChannelHandlerWrapper.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>
#include <cetty/service/ServiceFuture.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessagePtr.h>

namespace cetty {
namespace channel {
class Channel;
}
}

namespace cetty {
namespace shuttle {
namespace protobuf {

using namespace cetty::channel;
using namespace cetty::service;

class ProtobufProxyHandler : private boost::noncopyable {
public:
    typedef ChannelMessageContainer<ProtobufProxyMessagePtr,
            MESSAGE_BLOCK> InboundContainer;

    typedef InboundContainer::MessageQueue InboundQueue;

    typedef ChannelMessageHandlerContext<ProtobufProxyHandler,
            ProtobufProxyMessagePtr,
            VoidMessage,
            VoidMessage,
            ProtobufProxyMessagePtr,
            InboundContainer,
            VoidMessageContainer,
            VoidMessageContainer,
            InboundContainer> Context;

    typedef Context::Handler Handler;
    typedef Context::HandlerPtr HandlerPtr;

    typedef ServiceFuture<ProtobufProxyMessagePtr> ProxyFuture;

public:
    ProtobufProxyHandler() {}
    ~ProtobufProxyHandler() {}

public:
    void registerTo(Context& ctx) {
        context_ = &ctx;

        ctx.setChannelMessageUpdatedCallback(boost::bind(
                &ProtobufProxyHandler::messageUpdated,
                this,
                _1));
    }

private:
    void messageUpdated(ChannelHandlerContext& ctx);

    void messageReceived(ChannelHandlerContext& ctx,
                         const ProtobufProxyMessagePtr& msg);

    void onResponse(const ProxyFuture& future,
                    const ProtobufProxyMessagePtr& response);

private:
    Context* context_;
};

}
}
}

#endif //#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFHANDLER_H)

// Local Variables:
// mode: c++
// End:
