#if !defined(CETTY_CHANNEL_CHANNELINBOUNDBUFFERHANDLER_H)
#define CETTY_CHANNEL_CHANNELINBOUNDBUFFERHANDLER_H

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
#include <cetty/buffer/ChannelBufferPtr.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

template<class T>
class ChannelInboudBufferHandler {
public:
    typedef boost::shared_ptr<T> Ptr;

    typedef ChannelMessageHandlerContext<EchoClientHandler,
        ChannelBufferPtr,
        VoidMessage,
        VoidMessage,
        ChannelBufferPtr,
        ChannelBufferContainer,
        VoidMessageContainer,
        VoidMessageContainer,
        ChannelBufferContainer> Context;

public:
    virtual void registerTo(Context& ctx) {

    }

    virtual Ptr clone() const = 0;

    virtual void inboundBufferUpdated(ChannelHandlerContext& ctx) {
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
        const Exception& cause) {
    }

    void generator() {
        // Fill the outbound buffer up to 64KiB
        ctx_->outboundTransfer().unfoldAndAdd(content);
        ctx_->flush().addListener();

        ByteBuf out = ctx.nextOutboundByteBuffer();

        while (out.readableBytes() < 65536) {
            out.writeBytes(content);
        }

        // Flush the outbound buffer to the socket.
        // Once flushed, generate the same amount of traffic again.
        ctx.flush().addListener(GENERATE_TRAFFIC);
    }

private:
    Context* ctx_;
};

//pipeline.addLast(new ChannelMessageHandlerContext("xx", new ));
//pipeline.addLast(new ChannelMessageHandlerContext("xxx", loop, new xx));

class ChannelInboundBufferHandlerContext;

class ChannelInboundBufferHandler : public ChannelInboundHandler {
public:
    ChannelInboundBufferHandler();
    virtual ~ChannelInboundBufferHandler();

    virtual void channelOpen(ChannelHandlerContext& ctx);
    virtual void channelActive(ChannelHandlerContext& ctx);
    virtual void channelInactive(ChannelHandlerContext& ctx);
    virtual void messageUpdated(ChannelHandlerContext& ctx);

    virtual void beforeAdd(ChannelHandlerContext& ctx);
    virtual void afterAdd(ChannelHandlerContext& ctx);
    virtual void beforeRemove(ChannelHandlerContext& ctx);
    virtual void afterRemove(ChannelHandlerContext& ctx);

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                 const ChannelException& cause);

    virtual void userEventTriggered(ChannelHandlerContext& ctx,
                                    const boost::any& evt);

    virtual ChannelHandlerContext* createContext(const std::string& name,
            ChannelPipeline& pipeline,
            ChannelHandlerContext* prev,
            ChannelHandlerContext* next);

    virtual ChannelHandlerContext* createContext(const std::string& name,
            const EventLoopPtr& eventLoop,
            ChannelPipeline& pipeline,
            ChannelHandlerContext* prev,
            ChannelHandlerContext* next);

    virtual void setInboundChannelBuffer(const ChannelBufferPtr& buffer);

    const ChannelBufferPtr& getInboundChannelBuffer() const;

protected:
    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const ChannelBufferPtr& msg);

private:
    ChannelBufferPtr inboundBuffer;
};

inline const ChannelBufferPtr& ChannelInboundBufferHandler::getInboundChannelBuffer() const {
    return this->inboundBuffer;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELINBOUNDBUFFERHANDLER_H)

// Local Variables:
// mode: c++
// End:
