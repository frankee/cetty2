#if !defined(CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLERCONTEXT_H)
#define CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLERCONTEXT_H

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
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundMessageHandlerFwd.h>

namespace cetty {
namespace channel {

template<typename InboundInT>
class ChannelInboundMessageHandlerContext : public virtual ChannelHandlerContext {
public:
    typedef ChannelInboundMessageHandler<InboundInT> InboundHandler;
    typedef boost::intrusive_ptr<InboundHandler> InboundHandlerPtr;

public:
    ChannelInboundMessageHandlerContext(const std::string& name,
                                        ChannelPipeline& pipeline,
                                        const ChannelHandlerPtr& handler,
                                        ChannelHandlerContext* prev,
                                        ChannelHandlerContext* next)
        : ChannelHandlerContext(name, pipeline, handler, prev, next) {
        hasInboundMessageHandler = true;
        inboundMsgHandler = boost::dynamic_pointer_cast<InboundHandler>(handler);
        BOOST_ASSERT(inboundMsgHandler);
    }

    ChannelInboundMessageHandlerContext(const std::string& name,
                                        const EventLoopPtr& eventLoop,
                                        ChannelPipeline& pipeline,
                                        const ChannelHandlerPtr& handler,
                                        ChannelHandlerContext* prev,
                                        ChannelHandlerContext* next)
        : ChannelHandlerContext(name, eventLoop, pipeline, handler, prev, next) {
        hasInboundMessageHandler = true;
        inboundMsgHandler = boost::dynamic_pointer_cast<InboundHandler>(handler);
        BOOST_ASSERT(inboundMsgHandler);
    }

    virtual ~ChannelInboundMessageHandlerContext() {}

    virtual void addInboundMessage(const InboundInT& message) {
        if (eventloop->inLoopThread()) {
            inboundMsgHandler->addInboundMessage(message);
        }
        else {
            eventloop->post(boost::bind(
                                &ChannelInboundMessageHandlerContext<InboundInT>::addInboundMessage,
                                this,
                                message));
        }
    }

private:
    InboundHandlerPtr inboundMsgHandler;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:
