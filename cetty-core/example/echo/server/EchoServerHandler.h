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
/**
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/buffer/ChannelBufferFactory.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"

using namespace cetty::channel;
using namespace cetty::buffer;

/**
 * Handler implementation for the echo server.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class EchoServerHandler : public SimpleChannelUpstreamHandler {
public:
    EchoServerHandler() : transferredBytes(0) {
        out = ChannelBuffers::buffer(4096);
    }
    virtual ~EchoServerHandler() {}

    long getTransferredBytes() const {
        return transferredBytes;
    }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        // Send back the received message to the remote peer.
        //transferredBytes.addAndGet(((ChannelBuffer) e.getMessage()).readableBytes());
        //transferredBytes += e.getMessage()

        ChannelBufferPtr& buffer = e.getMessage().value<ChannelBufferPtr>();
        if (buffer) {
            static int readableBytes = buffer->readableBytes();

            //out->clear();
            //out.swap(buffer);
            buffer->clear();
            out->setIndex(0, readableBytes);
            e.getChannel()->write(ChannelMessage(out));
            
//             ChannelBufferPtr tmp = buffer->readBytes(readableBytes);
//             buffer->clear();
//             e.getChannel().write(tmp, false);
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        // Close the connection when an exception is raised.
//         logger.log(
//                 Level.WARNING,
//                 "Unexpected exception from downstream.",
//                 e.getCause());
        e.getChannel()->close();
    }

    virtual void writeCompleted(ChannelHandlerContext& ctx, const WriteCompletionEvent& e) {

    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new EchoServerHandler);
    }
    
    virtual std::string toString() const {
        return "EchoServerHandler";
    }

private:
    int transferredBytes;
    ChannelBufferPtr out;
};
