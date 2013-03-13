#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDCHANNEL_H)
#define CETTY_CHANNEL_EMBEDDED_EMBEDDEDCHANNEL_H

/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/Channel.h>
#include <cetty/channel/InetAddress.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>
#include <cetty/channel/embedded/EmbeddedEventLoop.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace embedded {

template<typename InboundOut,
         typename OutboundOut,
         typename InboundOutContainer,
         typename OutboundOutContainer>
class EmbeddedChannel : public Channel {
public:
    typedef EmbeddedChannel<InboundOut,
            OutboundOut,
            InboundOutContainer,
            OutboundOutContainer> ThisChannel;

    typedef ChannelMessageHandlerContext<ThisChannel*,
            InboundOut,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            InboundOutContainer,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer> InboundOutContext;

    typedef ChannelMessageHandlerContext<ThisChannel*,
            VoidMessage,
            VoidMessage,
            OutboundOut,
            VoidMessage,
            VoidMessageContainer,
            VoidMessageContainer,
            OutboundOutContainer,
            VoidMessageContainer> OutboundOutContext;

public:
    EmbeddedChannel()
        : Channel(ChannelPtr(), new EmbeddedEventLoop),
          lastException_() {
    }

    template<typename T>
    EmbeddedChannel(typename const ChannelHandlerWrapper<T>::HandlerPtr& handler)
        : Channel(ChannelPtr(),
                  new EmbeddedEventLoop),
        lastException_() {
        contexts_.push_back(
            new typename ChannelHandlerWrapper<T>::Handler::Context("1", t));
    }

    virtual ~EmbeddedChannel() {
        if (lastException_) {
            delete lastException_;
        }
    }

    template<typename T, typename MessageT>
    bool writeInbound(const T& data) {
        ChannelPipeline& pipeline = pipeline();
        pipeline.addInboundMessage<T, MessageT>(data);
        pipeline.fireMessageUpdated();

        checkException();
        return hasInboundOut();
    }

    template<typename T>
    bool writeInboundMessage(const T& data) {
        return writeInbound<T, MESSAGE_BLOCK>(data);
    }

    bool writeInboundBuffer(const ChannelBufferPtr& data) {
        return writeInbound<ChannelBufferPtr, MESSAGE_STREAM>(data);
    }

    InboundOut readInbound() {
        if (hasInboundOut()) {
            typename InboundOutContainer::MessageQueue& inboundOutQueue =
                inboundOutContainer_->getMessages();

            InboundOut out = inboundOutQueue.front();
            inboundOutQueue.pop_front();
            return out;
        }
        else {
            return InboundOut();
        }
    }

    OutboundOut readOutbound() {
        typename OutboundOutContainer::MessageQueue& outboundQueue =
            outboundOutContainer_->getMessages();

        if (!outboundQueue.empty()) {
            OutboundOut out = outboundQueue.front();
            outboundQueue.pop_front();
            return out;
        }

        return OutboundOut();
    }

    template<typename T, typename MessageT>
    bool writeOutbound(const T& msg) {
        write<T, MessageT>(msg);
        checkException();
        return hasOutboundOut();
    }

    template<typename T>
    bool writeOutboundMessage(const T& message) {
        return writeOutbound<T, MESSAGE_BLOCK>(message);
    }

    bool writeOutboundBuffer(const ChannelBufferPtr& buffer) {
        return writeOutbound<ChannelBufferPtr, MESSAGE_STREAM>(buffer);
    }

    bool finish() {
        close();
        checkException();
        return hasInboundOut() || hasOutboundOut();
    }

protected:
    bool hasInboundOut() const {
        return inboundOutContainer_ && !inboundOutContainer_->empty();
    }

    bool hasOutboundOut() const {
        return outboundOutContainer_ && !outboundOutContainer_->empty();
    }

    virtual bool doBind(const InetAddress& localAddress) {
        return true; // NOOP
    }

    virtual bool doDisconnect() {
        return doClose();
    }

    virtual bool doClose() {
        return true;
    }

private:
    void checkException() {
        Exception* t = lastException_;

        if (t == NULL) {
            return;
        }

        lastException_ = NULL;
        throw ChannelException(t->what(), t->getCode());
    }

    virtual void doInitialize() {
        ChannelPipeline& pipeline = Channel::pipeline();

        pipeline.addLast<ThisChannel*>("inboundOut",
                                       this,
                                       boost::bind(&EmbeddedChannel::registerInboundTo,
                                               this,
                                               _1,
                                               _2));

        pipeline.setHead<ThisChannel*>("outboutOut",
                                       this,
                                       boost::bind(&EmbeddedChannel::registerOutboundTo,
                                               this,
                                               _1,
                                               _2));
    }

    void exceptionCaught(ChannelHandlerContext& ctx,
                         const ChannelException& cause) {
        if (lastException_ == NULL) {
            lastException_ = new Exception(cause);
        }
        else {
            LOG_WARN <<  "More than one exception was raised. "
                     "Will report only the first one and log others.";
        }
    }

    void onOpen(ChannelHandlerContext& ctx) {
        setActived();
        pipeline->fireChannelActive();
    }

    void registerInboundTo(const ChannelPtr& channel, InboundOutContext& ctx) {
        inboundOutContainer_ = ctx.inboundContainer();
        ctx.setExceptionCallback(boost::bind(
                                     &ThisChannel::exceptionCaught,
                                     this,
                                     _1,
                                     _2));

        ctx.setChannelOpenCallback(boost::bind(
                                       &ThisChannel::onOpen,
                                       this,
                                       _1));
    }

    void registerOutboundTo(const ChannelPtr& channel, OutboundOutContext& ctx) {
        outboundOutContainer_ = ctx.outboundContainer();
    }

private:
    Exception* lastException_;

    InboundOutContainer* inboundOutContainer_;
    OutboundOutContainer* outboundOutContainer_;

    std::vector<ChannelHandlerContext*> contexts_;
};

template<typename OutboundOut, typename OutboundOutContainer> inline
ChannelBufferPtr EmbeddedChannel<ChannelBufferPtr,
                 OutboundOut,
                 ChannelBufferContainer,
OutboundOutContainer>::readInbound() {
    const ChannelBufferPtr& out = inboundOutContainer_->getMessages();
    return out;
}

template<typename InboundOut, typename InboundOutContainer> inline
ChannelBufferPtr EmbeddedChannel<InboundOut,
                 ChannelBufferPtr,
                 InboundOut,
                 InboundOutContainer,
ChannelBufferContainer>::readOutbound() {
    const ChannelBufferPtr& out = outboundOutContainer_->getMessages();
    return out;
}

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDCHANNEL_H)

// Local Variables:
// mode: c++
// End:
