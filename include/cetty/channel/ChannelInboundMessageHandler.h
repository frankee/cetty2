#if !defined(CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLER_H)
#define CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLER_H

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

#include <cetty/channel/ChannelInboundHandler.h>
#include <cetty/channel/ChannelInboundMessageHandlerContext.h>

namespace cetty {
namespace channel {

template<typename InboundInT>
class ChannelInboundMessageHandler : public ChannelInboundHandler {
public:
    typedef ChannelInboundMessageHandlerContext<InboundInT> MessageContext;

public:
    ChannelInboundMessageHandler() {}
    virtual ~ChannelInboundMessageHandler() {}

    virtual void channelCreated(ChannelHandlerContext& ctx) {
        ctx.fireChannelCreated();
    }

    virtual void channelActive(ChannelHandlerContext& ctx) {
        ctx.fireChannelActive();
    }

    virtual void channelInactive(ChannelHandlerContext& ctx) {
        ctx.fireChannelInactive();
    }

    virtual void writeCompleted(ChannelHandlerContext& ctx) {
        ctx.fireWriteCompleted();
    }

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        MessageContext* context = 
            ctx.inboundMessageHandlerContext<MessageContext>();

        messageUpdated(*context);
    }

    virtual void beforeAdd(ChannelHandlerContext& ctx) {}
    virtual void afterAdd(ChannelHandlerContext& ctx) {}
    virtual void beforeRemove(ChannelHandlerContext& ctx) {}
    virtual void afterRemove(ChannelHandlerContext& ctx) {}

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                 const ChannelException& cause) {
        ctx.fireExceptionCaught(cause);
    }

    virtual void userEventTriggered(ChannelHandlerContext& ctx,
                                    const UserEvent& evt) {
        ctx.fireUserEventTriggered(evt);
    }

    virtual ChannelHandlerContext* createContext(const std::string& name,
            ChannelPipeline& pipeline,
            ChannelHandlerContext* prev,
            ChannelHandlerContext* next) {
        return new MessageContext(name,
                pipeline,
                shared_from_this(),
                prev,
                next);
    }

    virtual ChannelHandlerContext* createContext(const std::string& name,
            const EventLoopPtr& eventLoop,
            ChannelPipeline& pipeline,
            ChannelHandlerContext* prev,
            ChannelHandlerContext* next) {
        return new MessageContext(name,
                eventLoop,
                pipeline,
                shared_from_this(),
                prev,
                next);
    }

protected:
    virtual void messageUpdated(MessageContext& ctx) {
        MessageContext::MessageQueue in = ctx.getInboundMessageQueue();

        while (!in.empty()) {
            InboundInT msg = in.front();

            if (!msg) {
                break;
            }

            try {
                messageReceived(ctx, msg);
            }
            catch (const ChannelException& e) {
                ctx.fireExceptionCaught(e);
            }
            catch (const std::exception& e) {
                ctx.fireExceptionCaught(ChannelException(e.what()));
            }

            in.pop_front();
        }
    }

    virtual void messageReceived(ChannelHandlerContext& ctx,
        const InboundInT& msg) {
            throw ChannelException("you must implement the messageReceived method.");
    }
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
