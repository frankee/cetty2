#if !defined(CETTY_CHANNEL_CHANNELSINKHANDLER_H)
#define CETTY_CHANNEL_CHANNELSINKHANDLER_H

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

#include <cetty/channel/ChannelOutboundBufferHandler.h>

namespace cetty {
namespace channel {

class AbstractChannel;

class ChannelPipelineBridgeHandler : private boost::noncopyable {
public:
    typedef ChannelMessageHandlerContext<ChannelPipelineBridgeHandler,
        void,
        ChannelBufferPtr,
        ChannelBufferPtr,
        void,
        void,
        ChannelBufferContainer,
        ChannelBufferContainer,
        void> Context;

    typedef ChannelHandlerContext::BindFunctor BindFunctor;

    typedef boost::shared_ptr<ChannelPipelineBridgeHandler> Ptr;

public:
    void registerTo(ChannelHandlerContext& ctx) {
        ctx.setBindFunctor(binder_);
    }

    const ChannelBufferPtr& inboundReadBuffer();

    
private:
    BindFunctor binder_;

};

class ChannelSinkHandler : public ChannelOutboundBufferHandler {
public:
    typedef boost::intrusive_ptr<AbstractChannel> AbstractChannelPtr;

public:
    ChannelSinkHandler() : channel() {}
    virtual ~ChannelSinkHandler() {}

    virtual void bind(ChannelHandlerContext& ctx,
                      const SocketAddress& localAddress,
                      const ChannelFuturePtr& future);

    virtual void connect(ChannelHandlerContext& ctx,
                         const SocketAddress& remoteAddress,
                         const SocketAddress& localAddress,
                         const ChannelFuturePtr& future);

    virtual void disconnect(ChannelHandlerContext& ctx,
                            const ChannelFuturePtr& future);

    virtual void close(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                 const ChannelException& cause);

    virtual void userEventTriggered(ChannelHandlerContext& ctx,
                                    const boost::any& evt);

    virtual ChannelHandlerPtr clone();

    virtual std::string toString() const;

protected:
    bool ensureOpen(const ChannelFuturePtr& future);

    void closeIfClosed();

private:
    void ensureChannelSet(ChannelHandlerContext& ctx);

private:
    AbstractChannelPtr channel;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELSINKHANDLER_H)

// Local Variables:
// mode: c++
// End:
