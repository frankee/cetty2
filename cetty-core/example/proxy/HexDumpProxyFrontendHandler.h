#if !defined(PROXY_HEXDUMPPROXYFRONTENDHANDLER_H)
#define PROXY_HEXDUMPPROXYFRONTENDHANDLER_H

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

#include <cetty/channel/ChannelHandlerContext.h>

class HexDumpProxyFrontendHandler : private boost::noncopyable {
public:
    HexDumpProxyFrontendHandler(const std::string& remoteHost,
                                int remotePort)
        : remotePort(remotePort),
          remoteHost(remoteHost) {
    }

    virtual void channelActive(ChannelHandlerContext& ctx) {
        // TODO: Suspend incoming traffic until connected to the remote host.
        //       Currently, we just keep the inbound traffic in the client channel's outbound buffer.
        const ChannelPtr& inboundChannel = ctx.channel();

        // Start the connection attempt.
        AbstractBootstrap b = new AbstractBootstrap();
        b.group(inboundChannel.eventLoop())
        .channel(NioSocketChannel.class)
        .remoteAddress(remoteHost, remotePort)
        .handler(new HexDumpProxyBackendHandler(inboundChannel));

        ChannelFuture f = b.connect();
        outboundChannel = f.channel();
        f.addListener(boost::bind(
                          &HexDumpProxyFrontendHandler::handleBackendConnection,
                          this,
                          _1));
    }

    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const ChannelBufferPtr& in) {
        ByteBuf out = outboundChannel.outboundByteBuffer();
        out.discardReadBytes();
        out.writeBytes(in);
        in.clear();

        if (outboundChannel.isActive()) {
            outboundChannel.flush();
        }
    }

    virtual void channelInactive(ChannelHandlerContext& ctx) {
        if (outboundChannel) {
            closeOnFlush(outboundChannel);
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                 const ChannelException& cause) {
        //cause.printStackTrace();
        closeOnFlush(ctx.channel());
    }

    /**
     * Closes the specified channel after all queued write requests are flushed.
     */
    static void closeOnFlush(const ChannelPtr& ch) {
        if (ch->isActive()) {
            ch->flush()->addListener(ChannelFutureListener::CLOSE);
        }
    }

private:
    void handleBackendConnection(const ChannelFuturePtr& future) {
        if (future->isSuccess()) {
            // Connection attempt succeeded:
            // TODO: Begin to accept incoming traffic.
        }
        else {
            // Close the connection if the connection attempt has failed.
            inboundChannel.close();
        }
    }

private:
    int remotePort;
    std::string remoteHost;

    ChannelPtr outboundChannel;
};

#endif //#if !defined(PROXY_HEXDUMPPROXYFRONTENDHANDLER_H)

// Local Variables:
// mode: c++
// End:
