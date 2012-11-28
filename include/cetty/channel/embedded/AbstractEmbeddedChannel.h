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
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/NullChannel.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/AbstractChannel.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/DefaultChannelConfig.h>
#include <cetty/channel/ChannelInboundBufferHandler.h>
#include <cetty/channel/ChannelInboundMessageHandler.h>

#include <cetty/channel/embedded/EmbeddedEventLoop.h>
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
    friend class LastInboundMessageHandler;
    friend class LastInboundBufferHandler;

    class LastInboundMessageHandler : public ChannelInboundMessageHandler<InboundOutT> {
    public:
        LastInboundMessageHandler(AbstractEmbeddedChannel& channel)
            : channel(channel) {
            channel.lastInboundMessageQueue = &inboundQueue;
        }

        virtual ChannelHandlerPtr clone() {
            return new LastInboundMessageHandler(channel);
        }

        virtual std::string toString() const {
            return "LastInboundMessageHandler";
        }

        virtual void messageUpdated(ChannelHandlerContext& ctx) {
            // Do nothing.
        }

        virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                     const ChannelException& cause) {
            if (channel.lastException == NULL) {
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

        virtual ChannelHandlerPtr clone() {
            return new LastInboundBufferHandler(channel);
        }

        virtual std::string toString() const {
            return "LastInboundBufferHandler";
        }

        virtual void messageUpdated(ChannelHandlerContext& ctx) {
            // Do nothing.
        }

    protected:
        virtual void setInboundChannelBuffer(const ChannelBufferPtr& buffer) {
            buffer->readBytes(channel.lastInboundChannelBuffer);
        }

    private:
        AbstractEmbeddedChannel& channel;
    };

    typedef typename LastInboundMessageHandler::MessageQueue InboundMessageQueue;

public:
    AbstractEmbeddedChannel(const ChannelHandlerPtr& handler)
        : AbstractChannel(new EmbeddedEventLoop,
                          ChannelPtr(),
                          NullChannel::instance()->getFactory(),
                          ChannelPipelines::pipeline(handler)),
        localAddress(new EmbeddedSocketAddressImpl),
        remoteAddress(new EmbeddedSocketAddressImpl),
        lastInboundMessageQueue(),
        lastException(),
        state() {
        init();
    }

    AbstractEmbeddedChannel(const ChannelHandlerPtr& handler1,
                            const ChannelHandlerPtr& handler2)
        : AbstractChannel(new EmbeddedEventLoop,
                          ChannelPtr(),
                          NullChannel::instance()->getFactory(),
                          ChannelPipelines::pipeline(handler1, handler2)),
        localAddress(new EmbeddedSocketAddressImpl),
        remoteAddress(new EmbeddedSocketAddressImpl),
        lastInboundMessageQueue(),
        lastException(),
        state() {
        init();
    }

    AbstractEmbeddedChannel(const ChannelHandlerPtr& handler1,
                            const ChannelHandlerPtr& handler2,
                            const ChannelHandlerPtr& handler3)
        : AbstractChannel(new EmbeddedEventLoop,
                          ChannelPtr(),
                          NullChannel::instance()->getFactory(),
                          ChannelPipelines::pipeline(handler1, handler2, handler3)),
        localAddress(new EmbeddedSocketAddressImpl),
        remoteAddress(new EmbeddedSocketAddressImpl),
        lastInboundMessageQueue(),
        lastException(),
        state() {
        init();
    }

    AbstractEmbeddedChannel(const ChannelPipelinePtr& pipeline)
        : AbstractChannel(new EmbeddedEventLoop,
                          ChannelPtr(),
                          NullChannel::instance()->getFactory(),
                          pipeline),
        localAddress(new EmbeddedSocketAddressImpl),
        remoteAddress(new EmbeddedSocketAddressImpl),
        lastInboundMessageQueue(),
        lastException(),
        state() {
        init();
    }

    virtual ~AbstractEmbeddedChannel() {
    }

    virtual ChannelConfig& config() { return config; }
    virtual const ChannelConfig& config() const { return config; }

    virtual bool isActive(void) const {
        return state == 1;
    }

    virtual bool isOpen() const {
        return state < 2;
    }

    virtual const SocketAddress& localAddress(void) const {
        return localAddress;
    }
    virtual const SocketAddress& remoteAddress(void) const {
        return remoteAddress;
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
        ChannelPipelinePtr pipeline = pipeline();
        pipeline->addInboundMessage(data);
        pipeline->fireMessageUpdated();

        checkException();
        return hasInboundOut();
    }

    template<>
    bool writeInbound<ChannelBufferPtr>(const ChannelBufferPtr& data) {
        ChannelPipelinePtr pipeline = pipeline();
        pipeline->setInboundChannelBuffer(data);
        pipeline->fireMessageUpdated();

        checkException();
        return hasInboundOut();
    }

    bool readInbound(InboundOutT* inboundOut) {
        if (lastInboundMessageQueue && !lastInboundMessageQueue->empty()) {
            *inboundOut = lastInboundMessageQueue->front();
            lastInboundMessageQueue->pop_front();
            return true;
        }

        return false;
    }

    bool readInbound(ChannelBufferPtr* inboundOut) {
        if (lastInboundChannelBuffer->readable()) {
            *inboundOut = lastInboundChannelBuffer->readBytes(
                              lastInboundChannelBuffer->readableBytes());

            lastInboundChannelBuffer->clear();

            return true;
        }

        return false;
    }

    void checkException() {
        Exception* t = lastException;

        if (t == NULL) {
            return;
        }

        lastException = NULL;
        throw ChannelException(t->what(), t->getCode());
    }

protected:
    bool hasInboundOut() {
        return (lastInboundChannelBuffer && lastInboundChannelBuffer->readable())
               || (lastInboundMessageQueue && !lastInboundMessageQueue->empty());
    }

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
    void init() {
        
        ChannelPipelinePtr pipeline = AbstractChannel::pipeline();
        pipeline->addLast("lastMesssage", new LastInboundMessageHandler(*this));
        pipeline->addLast("lastBuffer", new LastInboundBufferHandler(*this));
        AbstractChannel::setPipeline(pipeline);
        pipeline->fireChannelOpen();
        pipeline->fireChannelActive();
        state = 1;
    }

private:
    DefaultChannelConfig config;

    SocketAddress localAddress;
    SocketAddress remoteAddress;

    InboundMessageQueue* lastInboundMessageQueue;
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
