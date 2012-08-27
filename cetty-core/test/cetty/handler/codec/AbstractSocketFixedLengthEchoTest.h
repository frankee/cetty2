#if !defined(CETTY_HANDLER_CODEC_FRAME_ABSTRACTSOCKETFIXEDLENGTHECHOTEST_H)
#define CETTY_HANDLER_CODEC_FRAME_ABSTRACTSOCKETFIXEDLENGTHECHOTEST_H

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

#include "boost/any.hpp"
#include "boost/thread.hpp"
#include "boost/assert.hpp"
#include "gtest/gtest.h"

#include "cetty/util/Random.h"
#include "cetty/util/Exception.h"
#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"

#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelFactory.h"
#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"

#include "cetty/bootstrap/ClientBootstrap.h"
#include "cetty/bootstrap/ServerBootstrap.h"

#include "cetty/handler/codec/frame/FixedLengthFrameDecoder.h"

namespace cetty { namespace handler { namespace codec { namespace frame { 

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::bootstrap;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::codec::frame;

/**
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2119 $, $Date: 2010-02-01 20:46:09 +0900 (Mon, 01 Feb 2010) $
 *
 */
class AbstractSocketFixedLengthEchoTest : public testing::Test {
public:
    AbstractSocketFixedLengthEchoTest() {}
    virtual ~AbstractSocketFixedLengthEchoTest() {}

    void SetUp() {
        time_t t;
        time(&t);
        random.setSeed(t);
        random.nextBytes(data, DATA_LENGTH);
    }

    void TearDown() {
    }

public:
    void testFixedLengthEcho() {
        ServerBootstrap sb(ChannelFactoryPtr(newServerSocketChannelFactory(1)));
        ClientBootstrap cb(ChannelFactoryPtr(newClientSocketChannelFactory(1)));

        EchoHandler* sh = new EchoHandler(this, true);
        EchoHandler* ch = new EchoHandler(this, false);

        sb.getPipeline()->addLast("decoder", ChannelHandlerPtr(new FixedLengthFrameDecoder(1024)));
        sb.getPipeline()->addAfter("decoder", "handler", ChannelHandlerPtr(sh));
        cb.getPipeline()->addLast("decoder", ChannelHandlerPtr(new FixedLengthFrameDecoder(1024)));
        cb.getPipeline()->addAfter("decoder", "handler", ChannelHandlerPtr(ch));

        Channel* sc = sb.bind(SocketAddress(IpAddress::IPv4));
        int port = sc->getLocalAddress().port();

        ChannelFuturePtr ccf = cb.connect(SocketAddress("127.0.0.1", port));
        BOOST_ASSERT(ccf->awaitUninterruptibly().isSuccess());

        Channel& cc = ccf->getChannel();
        for (int i = 0; i < DATA_LENGTH;) {
            int length = std::min(random.nextInt(1024 * 3), DATA_LENGTH - i);
            cc.write(
                ChannelMessage(boost::any(
                    ChannelBuffers::wrappedBuffer(Array(data,DATA_LENGTH), i, length))));
            i += length;
        }

        while (ch->counter < DATA_LENGTH) {
            if (sh->exception) break;
            if (ch->exception) break;
            boost::this_thread::sleep(boost::posix_time::millisec(1000));
        }

        while (sh->counter < DATA_LENGTH) {
            if (sh->exception) break;
            if (ch->exception) break;
            boost::this_thread::sleep(boost::posix_time::millisec(1000));
        }

        //sh->channel->close()->awaitUninterruptibly();
        //ch->channel->close()->awaitUninterruptibly();

        cc.close()->awaitUninterruptibly();
        sc->close()->awaitUninterruptibly();

        if (sh->exception) {
            sh->exception->rethrow();
        }
        if (ch->exception) {
            ch->exception->rethrow();
        }
    }

protected:
    virtual ChannelFactory* newServerSocketChannelFactory(int ioThreadCount) = 0;
    virtual ChannelFactory* newClientSocketChannelFactory(int ioThreadCount) = 0;

private:
    class EchoHandler : public cetty::channel::SimpleChannelUpstreamHandler {
    public:
        EchoHandler(AbstractSocketFixedLengthEchoTest* test, bool isServer)
            : channel(NULL), exception(NULL), counter(0), test(test), isServer(isServer) {
        }
        virtual ~EchoHandler() {
            if (exception) {
                delete exception;
            }
        }

        virtual bool sharable() const {
            return true;
        }
        virtual ChannelHandlerPtr clone() const {
            ChannelHandlerPtr ptr(new EchoHandler(test, isServer));
            return ptr;
        }

        virtual std::string toString() const { return "EchoHandler"; }

        void channelOpen(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
            channel = &(e.getChannel());
        }

        void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
            ChannelBufferPtr m = e.getMessage().value<ChannelBufferPtr>();
            ASSERT_EQ(1024, m->readableBytes());

            ChannelBufferPtr buffer = m->copy();

            int lastIdx = counter;
            for (int i = 0; i < 1024; i++) {
                //ASSERT_EQ(test->getData(i + lastIdx), buffer->getByte(i));
            }

            if (channel->getParent() != NULL) { //at server
                channel->write(ChannelMessage(boost::any(buffer)));
            }
            //m->discardReadBytes();
            counter += 1024;

            if (isServer) {
                printf("Server: Received %d Bytes\n", counter);
            }
            else {
                printf("Client: Received %d Bytes\n", counter);
            }
        }

        void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
            exception = new Exception(e.getCause());
            e.getChannel().close();
        }

        bool isServer;
        Channel* channel;
        int counter;
        Exception* exception;

        AbstractSocketFixedLengthEchoTest* test;
    };

    char* getData() { return data; }
    char  getData(int index) { return data[index]; }

private:
    static const int  DATA_LENGTH = 8192/*1048576*/;

    Random random;
    char data[DATA_LENGTH];
};

}}}}

#endif //#if !defined(CETTY_HANDLER_CODEC_FRAME_ABSTRACTSOCKETFIXEDLENGTHECHOTEST_H)
