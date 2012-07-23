#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELSINK_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELSINK_H

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

#include <cetty/channel/AbstractChannelSink.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

    class AsioSocketChannel;

class AsioSocketChannelSink : public cetty::channel::AbstractChannelSink {
public:
    AsioSocketChannelSink(AsioSocketChannel& channel) {}
    virtual ~AsioSocketChannelSink() {}

public:
    virtual void connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {

                if (eventLoop().inEventLoop()) {
                    if (!ensureOpen(future)) {
                        return;
                    }

                    try {
                        if (connectFuture != null) {
                            throw new IllegalStateException("connection attempt already made");
                        }
                        connectFuture = future;

                        channel.doConnect(remoteAddress, localAddress, future);

                        // Schedule connect timeout.
                        int connectTimeoutMillis = config().getConnectTimeoutMillis();
                        if (connectTimeoutMillis > 0) {
                            connectTimeoutFuture = eventLoop().schedule(new Runnable() {
                                @Override
                                    public void run() {
                                        if (connectTimeoutException == null) {
                                            connectTimeoutException = new ConnectException("connection timed out");
                                        }
                                        ChannelFuture connectFuture = AbstractAioChannel.this.connectFuture;
                                        if (connectFuture != null &&
                                            connectFuture.setFailure(connectTimeoutException)) {
                                                pipeline().fireExceptionCaught(connectTimeoutException);
                                                close(voidFuture());
                                        }
                                }
                            }, connectTimeoutMillis, TimeUnit.MILLISECONDS);
                        }

                    } catch (Throwable t) {
                        future.setFailure(t);
                        pipeline().fireExceptionCaught(t);
                        closeIfClosed();
                    }
                }
                else {
                    eventLoop().execute(new Runnable() {
                        @Override
                            public void run() {
                                connect(remoteAddress, localAddress, future);
                        }
                    });
                }
        }

    virtual void flush(const ChannelFuturePtr& future) {

    }

private:
        void connectFailed(Throwable t) {
            connectFuture.setFailure(t);
            pipeline().fireExceptionCaught(t);
            closeIfClosed();
        }

        void connectSuccess() {
            assert eventLoop().inEventLoop();
            assert connectFuture != null;
            try {
                boolean wasActive = isActive();
                connectFuture.setSuccess();
                if (!wasActive && isActive()) {
                    pipeline().fireChannelActive();
                }
            } catch (Throwable t) {
                connectFuture.setFailure(t);
                pipeline().fireExceptionCaught(t);
                closeIfClosed();
            } finally {
                connectTimeoutFuture.cancel(false);
                connectFuture = null;
            }
        }
    }

    virtual void flush(const ChannelFuturePtr& future) = 0;

private:
    AsioSocketChannel& channel;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELSINK_H)

// Local Variables:
// mode: c++
// End:
