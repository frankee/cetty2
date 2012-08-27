#if !defined(CETTY_CHANNEL_EMBEDDED_ABSTRACTEMBEDDEDCHANNEL_H)
#define CETTY_CHANNEL_EMBEDDED_ABSTRACTEMBEDDEDCHANNEL_H

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
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 *
 */

#include <cetty/channel/AbstractChannel.h>
#include <cetty/channel/embedded/EmbeddedSocketAddressImpl.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace embedded {

using namespace cetty::channel;

template<typename InboundOutT,
         typename OutboundOutT>
class AbstractEmbeddedChannel : public AbstractChannel {
public:
    class LastInboundMessageHandler : public ChannelInboundMessageHandler<InboundOutT> {
        LastInboundMessageHandler(AbstractEmbeddedChannel& channel)
        : channel(channel) {
            channel.lastInboundMessageQueue = &inboundQueue;
        }

        virtual void ChannelHandlerPtr clone();

        virtual std::string toString() const;

        virtual void messageUpdated(ChannelHandlerContext& ctx) {
            // Do nothing.
        }

        virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                     const ChannelException& cause) {
            if (channel.lastException == null) {
                channel.lastException = new Exception(cause);
            }
            else {
                LOG_WARN_E(cause) <<  "More than one exception was raised. "
                                  "Will report only the first one and log others.";
            }
        }

    private:
        AbstractEmbeddedChannel& channel;
    };

    class LastInboundBufferHandler : public ChannelInboundBufferHandler {
    public:
        LastInboundBufferHandler(AbstractEmbeddedChannel& channel)
            : channel(channel) {}

        virtual void ChannelHandlerPtr clone();

        virtual std::string toString() const;

        virtual void messageUpdated(ChannelHandlerContext& ctx) {}

    protected:
        virtual void setInboundChannelBuffer(const ChannelBufferPtr& buffer) {
            buffer->readBytes(channel.lastInboundChannelBuffer);
        }

    private:
        AbstractEmbeddedChannel& channel;
    };

    typedef LastInboundMessageHandler::MessageQueue InboundMessageQueue;

    friend class LastInboundMessageHandler;
    friend class LastInboundBufferHandler;

public:
    AbstractEmbeddedChannel(const ChannelHandlerPtr& handler)
        : localAddress() {

    }
    AbstractEmbeddedChannel(const ChannelHandlerPtr& handler1,
                            const ChannelHandlerPtr& handler2);

    AbstractEmbeddedChannel(const ChannelHandlerPtr& handler1,
                            const ChannelHandlerPtr& handler2,
                            const ChannelHandlerPtr& handler3);

    AbstractEmbeddedChannel(const ChannelPipelinePtr& pipeline);

    AbstractEmbeddedChannel(const EventLoopPtr& eventLoop,
                            const ChannelPtr& parent,
                            const ChannelFactoryPtr& factory,
                            const ChannelPipelinePtr& pipeline)
        : AbstractChannel(eventLoop, parent, factory, pipeline) {

        pipeline->addLast("lastMesssage", new LastInboundMessageHandler(*this));
        pipeline->addLast("lastBuffer", new LastInboundBufferHandler(*this));
    }

    virtual ChannelConfig& getConfig() { return config; }
    virtual const ChannelConfig& getConfig() const { return config; }

    bool isOpen() {
        return state < 2;
    }

    bool isActive() {
        return state == 1;
    }

    InboundMessageQueue& getLastInboundMessageQueue() {
        BOOST_ASSERT(lastInboundMessageQueue);
        return *lastInboundMessageQueue;
    }

    const ChannelBufferPtr& getLastInboundChannelBuffer() {
        return lastInboundChannelBuffer;
    }

    template<typename T>
    bool writeInbound(const T& data) {
        inboundBuffer().add(msg);
        pipeline()->fireMessageUpdated();

        checkException();
        return getLastInboundChannelBuffer()->readable()
            || !getLastInboundMessageQueue().empty();
    }

    template<>
    bool writeInbound<ChannelBufferPtr>(const ChannelBufferPtr& data) {
        inboundBuffer().writeBytes(data);
        pipeline()->fireMessageUpdated();

        checkException();
        return getLastInboundChannelBuffer()->readable()
            || !getLastInboundMessageQueue().empty();
    }

    InboundOutT readInbound() {
        if (lastInboundChannelBuffer->readable()) {
            buffer = lastInboundChannelBuffer->readBytes(
                         lastInboundChannelBuffer->readableBytes());

            lastInboundChannelBuffer->clear();

            return buffer;
        }

        if (lastInboundMessageQueue && !lastInboundMessageQueue->empty()) {
            InboundOutT = lastInboundMessageQueue->front();
            lastInboundMessageQueue->pop_front();
            return InboundOutT;
        }

        return InboundOutT();
    }

    void checkException() {
        Exception* t = lastException;

        if (t == NULL) {
            return;
        }

        lastException = NULL;
        throw ChannelException(*t);
    }

protected:
    void doBind(const SocketAddress& localAddress) {
        // NOOP
    }

    void doDisconnect() {
        doClose();
    }

    void doClose() {
        state = 2;
    }

private:
    DefaultChannelConfig config;
    SocketAddress localAddress;
    SocketAddress remoteAddress;

    LastInboundMessageHandler::MessageQueue* lastInboundMessageQueue;
    ChannelBufferPtr lastInboundChannelBuffer;

    Exception* lastException;

    int state; // 0 = OPEN, 1 = ACTIVE, 2 = CLOSED
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_ABSTRACTEMBEDDEDCHANNEL_H)

// Local Variables:
// mode: c++
// End:
