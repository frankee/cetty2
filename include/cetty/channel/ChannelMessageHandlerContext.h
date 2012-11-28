#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHANDLERCONTEXT_H)
#define CETTY_CHANNEL_CHANNELMESSAGEHANDLERCONTEXT_H

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

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelHandlerWrapper.h>

namespace cetty {
namespace channel {

template<class Handler,
         class InboundIn,
         class InboundOut,
         class OutboundIn,
         class OutboundOut,
         class InboundInContainer,
         class InboundOutContainer,
         class OutboundInContainer,
         class OutboundOutContainer>
class ChannelMessageHandlerContext : public ChannelHandlerContext {
public:
    typedef typename ChannelHandlerWrapper<Handler>::Handler Handler;
    typedef typename ChannelHandlerWrapper<Handler>::HandlerPtr HandlerPtr;

    typedef ChannelMessageTransfer<InboundOut, InboundOutContainer, TRANSFER_INBOUND> InboundTransfer;
    typedef ChannelMessageTransfer<OutboundOut, OutboundOutContainer, TRANSFER_OUTBOUND> OutboundTransfer;

public:
    InboundInContainer* inboundInContainer();
    //InboundOutContainer* inboundOutContainer();

    OutboundInContainer* outboundInContainer();
    //OutboundOutContainer* outboundOutContainer();

    InboundTransfer inboundTransfer();
    OutboundTransfer outboundTransfer();
};

template<class Handler>
class ChannelMessageHandlerContext<
    Handler,
    VoidMessage,
    VoidMessage,
    VoidMessage,
    VoidMessage,
    VoidMessageContainer,
    VoidMessageContainer,
    VoidMessageContainer,
        VoidMessageContainer> {

public:
    typedef boost::shared_ptr<Handler> HandlerPtr;

public:
    ChannelMessageHandlerContext(const std::string& name,
                                 const HandlerPtr& handler)
        : ChannelHandlerContext(name) {
        if (handler) {
            handler->registerTo(this);
        }
    }

    ChannelMessageHandlerContext(const std::string& name,
                                 const EventLoopPtr& eventLoop,
                                 const HandlerPtr& handler)
        : ChannelHandlerContext(name, eventLoop) {
        if (handler) {
            handler->registerTo(this);
        }
    }

    virtual boost::any getInboundMessageContainer() {
        return boost::any();
    }

    virtual boost::any getOutboundMessageContainer() {
        return boost::any();
    }
};

template<class Handler,
         class InboundIn,
         class InboundOut,
         class OutboundIn,
         class OutboundOut>
class ChannelMessageHandlerContext<Handler,
    InboundIn,
    InboundOut,
    OutboundIn,
    OutboundOut,
    ChannelMessageContainer<InboundIn>,
    ChannelMessageContainer<InboundOut>,
    ChannelMessageContainer<OutboundIn>,
        ChannelMessageContainer<OutboundOut> > {
public:
    typedef ChannelMessageContainer<InboundIn> InboundInContainer;
    typedef ChannelMessageContainer<OutboundIn> OutboundInContainer;

    typedef ChannelMessageTransfer<InboundOut, InboundOutContainer, TRANSFER_INBOUND> InboundTransfer;
    typedef ChannelMessageTransfer<OutboundOut, OutboundOutContainer, TRANSFER_OUTBOUND> OutboundTransfer;

public:
    ChannelMessageHandlerContext(const std::string& name, const Ptr& ptr)
        : ChannelHandlerContext(name),
          inboundTransfer_(*this),
          outboundTransfer_(*this) {
    }

    virtual attach() {
        inboundContainer_.setEventLoop(eventLoop());
        outboundContainer_.setEventLoop(eventLoop());
    }

    virtual void setNext(ChannelHandlerContext* ctx) {
        outboundTransfer_.resetNextContainer();
    }

    virtual void setBefore(ChannelHandlerContext* ctx) {
        inboundTransfer_.resetNextContainer();
    }

    virtual boost::any getInboundMessageContainer() {
        return boost::any(&inboundContainer_);
    }

    virtual boost::any& getOutboundMessageContainer() {
        return boost::any(&outboundTransfer_);
    }

private:
    InboundInContainer inboundContainer_;
    InboundOutContainer outboundContainer_;

    InboundTransfer inboundTransfer_;
    OutboundTransfer outboundTransfer_;
};

template<class Handler, class InboundOut>
class ChannelMessageHandlerContext<Handler, ChannelBufferPtr, InboundOut, VoidMessage, VoidMessage> {
public:
    typedef ChannelMessageContainer<ChannelBufferPtr> InboundContainer;
    typedef ChannelMessageTransfer<typename InOut> InboundTransfer;

public:
    InboundTransfer& inboundTransfer() { return inboundTransfer_; }

private:
    InboundContainer inboundContainer_;
    InboundTransfer inboundTransfer_;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:
