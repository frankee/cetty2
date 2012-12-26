#if !defined(PROXY_HEXDUMPPROXYBACKENDHANDLER_H)
#define PROXY_HEXDUMPPROXYBACKENDHANDLER_H

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

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundBufferHandlerAdapter.h>

using namespace cetty::buffer;

class HexDumpProxyBackendHandler : private boost::noncopyable {
public:
    HexDumpProxyBackendHandler(const ChannelPtr& inboundChannel) {
        this.inboundChannel = inboundChannel;
    }

    virtual void channelActive(ChannelHandlerContext& ctx) {
        ctx.flush();
    }

    virtual void messageReceived(ChannelHandlerContext& ctx, const ChannelBufferPtr& in) {
        ByteBuf out = inboundChannel.outboundByteBuffer();
        out.discardReadBytes();
        out.writeBytes(in);
        in.clear();
        inboundChannel.flush();
    }

    virtual void channelInactive(ChannelHandlerContext& ctx) {
        HexDumpProxyFrontendHandler.closeOnFlush(inboundChannel);
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {
        //cause.printStackTrace();
        HexDumpProxyFrontendHandler.closeOnFlush(ctx.channel());
    }

private:
    ChannelPtr inboundChannel;
};

#endif //#if !defined(PROXY_HEXDUMPPROXYBACKENDHANDLER_H)

// Local Variables:
// mode: c++
// End:
