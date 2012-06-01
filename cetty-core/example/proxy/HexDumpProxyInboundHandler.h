/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelFactory.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelFutureListener.h"

#include "cetty/buffer/ChannelBuffers.h"

#include "cetty/bootstrap/ClientBootstrap.h"

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::bootstrap;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class HexDumpProxyInboundHandler : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    HexDumpProxyInboundHandler(
        const ChannelFactoryPtr& cf, const std::string& remoteHost, int remotePort)
        : cf(cf), remoteHost(remoteHost), remotePort(remotePort), cb(cf) {
    }

    virtual ~HexDumpProxyInboundHandler() {}

    virtual void channelOpen(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        // Suspend incoming traffic until connected to the remote host.
        Channel& inboundChannel = e.getChannel();
        inboundChannel.setReadable(false);

        // Start the connection attempt.
        cb.getPipeline()->addLast(
            "handler", ChannelHandlerPtr(new OutboundHandler(e.getChannel())));
        ChannelFuturePtr f = cb.connect(SocketAddress(remoteHost, remotePort));

        outboundChannel = &(f->getChannel());
        f->setListener(boost::bind(
            &HexDumpProxyInboundHandler::handleChannelOpen,
            this,
            _1,
            boost::ref(inboundChannel)));
    }

    void handleChannelOpen(const ChannelFuturePtr& future, Channel& inboundChannel) {
        if (future->isSuccess()) {
            // Connection attempt succeeded:
            // Begin to accept incoming traffic.
            inboundChannel.setReadable(true);
        }
        else {
            // Close the connection if the connection attempt has failed.
            inboundChannel.close();
        }
    }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        const ChannelBufferPtr& msg = e.getMessage().value<ChannelBufferPtr>();
        printf(">>> %s\n", ChannelBuffers::hexDump(*msg).c_str());
        
        outboundChannel->write(msg->readBytes());
    }

    virtual void channelClosed(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        if (outboundChannel != NULL) {
            closeOnFlush(*outboundChannel);
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        //e.getCause().printStackTrace();
        //closeOnFlush(e.getChannel());
        e.getChannel().close();
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new HexDumpProxyInboundHandler(cf, remoteHost, remotePort));
    }

    virtual std::string toString() const {
        return "HexDumpProxyInboundHandler";
    }

    class OutboundHandler : public cetty::channel::SimpleChannelUpstreamHandler {
    public:
        OutboundHandler(Channel& inboundChannel) : inboundChannel(inboundChannel) {
        }

        virtual ~OutboundHandler() {}

        virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
            const ChannelBufferPtr& msg = e.getMessage().value<ChannelBufferPtr>();
            printf("<<< %s\n", ChannelBuffers::hexDump(*msg).c_str());

            inboundChannel.write(msg->readBytes());
        }

        virtual void channelClosed(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
            HexDumpProxyInboundHandler::closeOnFlush(inboundChannel);
        }

        virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
            //e.getCause().printStackTrace();
            //HexDumpProxyInboundHandler::closeOnFlush(e.getChannel());
            e.getChannel().close();
        }

        virtual ChannelHandlerPtr clone() {
            return ChannelHandlerPtr(new OutboundHandler(inboundChannel));
        }

        virtual std::string toString() const {
            return "OutboundHandler";
        }

    private:
        Channel& inboundChannel;
    };

    /**
     * Closes the specified channel after all queued write requests are flushed.
     */
    static void closeOnFlush(Channel& ch) {
        if (ch.isConnected()) {
            ch.write(ChannelBuffers::EMPTY_BUFFER)->addListener(ChannelFutureListener::CLOSE);
        }
    }

private:
    ChannelFactoryPtr cf;
    std::string remoteHost;
    int remotePort;

    ClientBootstrap cb;

    Channel* outboundChannel;
};
