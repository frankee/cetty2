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

#include "boost/any.hpp"
#include "boost/thread.hpp"
#include "boost/assert.hpp"
#include "gtest/gtest.h"

#include "cetty/util/Random.h"
#include "cetty/util/Exception.h"
#include "cetty/buffer/Array.h"
#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"

#include "cetty/channel/SocketAddress.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelFuture.h"
#include "cetty/channel/ChannelFactory.h"
#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/socket/asio/AsioClientSocketChannelFactory.h"
#include "cetty/channel/socket/asio/AsioServerSocketChannelFactory.h"

#include "cetty/bootstrap/ClientBootstrap.h"
#include "cetty/bootstrap/ServerBootstrap.h"


using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::bootstrap;
using namespace cetty::channel::socket::asio;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 */

class AsioSocketEchoTest : public testing::Test {
private:
	class EchoHandler;
	typedef boost::intrusive_ptr<EchoHandler> EchoHandlerPtr;

public:
	AsioSocketEchoTest() : data(buf, DATA_LENGTH) {}
	virtual ~AsioSocketEchoTest() {}

	void SetUp() {
		time_t t = time(0);
		random.setSeed(t);
		random.nextBytes(buf, DATA_LENGTH);
	}

	void TearDown() {
	}

	void testSimpleEcho() {
		ServerBootstrap sb(ChannelFactoryPtr(new AsioServerSocketChannelFactory()));
		ClientBootstrap cb(ChannelFactoryPtr(new AsioClientSocketChannelFactory()));

		EchoHandlerPtr sh(new EchoHandler(data));
		EchoHandlerPtr ch(new EchoHandler(data));

		sb.getPipeline()->addFirst("server-handler", boost::dynamic_pointer_cast<ChannelHandler>(sh));
		cb.getPipeline()->addFirst("client-handler", boost::dynamic_pointer_cast<ChannelHandler>(ch));

		Channel* sc = sb.bind(SocketAddress(IpAddress::IPv4, 0));
		int port = sc->getLocalAddress().port();

		ChannelFuturePtr ccf = cb.connect(SocketAddress("127.0.0.1", port));
		ASSERT_TRUE(ccf->awaitUninterruptibly().isSuccess());

		Channel& cc = ccf->getChannel();
		for (int i = 0; i < data.length();) {
			int length = std::min(random.nextInt(1024 * 64), data.length() - i);
            ChannelBufferPtr buf = ChannelBuffers::wrappedBuffer(data, i, length);
            for (int j = 0; j < buf->readableBytes(); ++j) {
                if (buf->getByte(j) != data[i + j]) {
                    printf("wrapbuffer has mistakes.\n");
                } 
            }
			cc.write(buf);
			i += length;
		}

		while (ch->counter < data.length()) {
			if (sh->exception != NULL) {
				break;
			}
			if (ch->exception != NULL) {
				break;
			}

			boost::this_thread::sleep(boost::posix_time::millisec(1));
		}

		while (sh->counter < data.length()) {
			if (sh->exception != NULL) {
				break;
			}
			if (ch->exception != NULL) {
				break;
			}

			boost::this_thread::sleep(boost::posix_time::millisec(1));
		}

		sh->channel->close()->awaitUninterruptibly();
		ch->channel->close()->awaitUninterruptibly();
		sc->close()->awaitUninterruptibly();

		if (sh->exception != NULL) {
			sh->exception->rethrow();
		}
		if (ch->exception != NULL) {
			ch->exception->rethrow();
		}
	}

private:
	class EchoHandler : public SimpleChannelUpstreamHandler {
	public:
		Channel* channel;
		const Exception* exception;
		volatile int counter;

		EchoHandler(Array& data)
			: channel(NULL), exception(NULL), counter(0), data(data) {
		}

		virtual ~EchoHandler() {}

		virtual std::string toString() const { return "EchoHandler"; }

		virtual ChannelHandlerPtr clone() { return shared_from_this(); }

		virtual void channelOpen(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
			channel = &(e.getChannel());
		}

		virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
			bool isclient = (channel->getParent() == NULL);

			ChannelBufferPtr m = e.getMessage().smartPointer<ChannelBuffer>();
            printf("%s, rIdx = %d, wIdx = %d, count = %d\n",
                    isclient ? "client" : "server", m->readerIndex(), m->writerIndex(), counter);

			int length = m->readableBytes();
			ChannelBufferPtr copied = m->readSlice();

			Array actual(copied->array().data() + copied->readerIndex(), length);
			int lastIdx = counter;
			for (int i = 0; i < actual.length(); i ++) {
                if (data[i + lastIdx] != actual[i]) {
                    printf("data[%d] = %d, <%d>\n", i + lastIdx, data[i+lastIdx], actual[i]);
                }
			}

			if (!isclient) {
				channel->write(copied);
			}

			counter += length;
		}

		virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
			exception = &(e.getCause());
			e.getChannel().close();
		}

	private:
		Array data;
	};

private:
	Random random;
	static const int DATA_LENGTH = 1048576;
	char buf[DATA_LENGTH];
	Array data;
};


TEST_F(AsioSocketEchoTest, testSimpleEcho) {
	testSimpleEcho();
}