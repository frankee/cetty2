#if !defined(CETTY_CHANNEL_ABSTRACTCHANNELSINK_H)
#define CETTY_CHANNEL_ABSTRACTCHANNELSINK_H

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
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/ChannelSink.h>

namespace cetty {
namespace channel {

/**
 * A skeletal {@link ChannelSink} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class AbstractChannelSink : public ChannelSink {
public:
    AbstractChannelSink();
    virtual ~AbstractChannelSink() {}

public:
    AbstractSink(AbstractChannel& channel) {}
    virtual ~AbstractSink() {}

    void bind(const SocketAddress& localAddress, const ChannelFuturePtr& future) {
        if (eventLoop().inEventLoop()) {
            if (!ensureOpen(future)) {
                return;
            }

            try {
                boolean wasActive = isActive();
                channel.doBind(localAddress);
                future.setSuccess();
                if (!wasActive && isActive()) {
                    pipeline.fireChannelActive();
                }
            } catch (Throwable t) {
                future.setFailure(t);
                pipeline.fireExceptionCaught(t);
                closeIfClosed();
            }
        } else {
            eventLoop().execute(new Runnable() {
                @Override
                    public void run() {
                        bind(localAddress, future);
                }
            });
        }
    }

    void disconnect(const ChannelFuturePtr& future) {
        if (eventLoop().inEventLoop()) {
            try {
                boolean wasActive = isActive();
                doDisconnect();
                future.setSuccess();
                if (wasActive && !isActive()) {
                    pipeline.fireChannelInactive();
                }
            } catch (Throwable t) {
                future.setFailure(t);
                closeIfClosed();
            }
        } else {
            eventLoop().execute(new Runnable() {
                @Override
                    public void run() {
                        disconnect(future);
                }
            });
        }
    }

    void close(ChannelFuture future) {
        if (eventLoop().inEventLoop()) {
            boolean wasActive = isActive();
            if (closeFuture.setClosed()) {
                try {
                    doClose();
                    future.setSuccess();
                } catch (Throwable t) {
                    future.setFailure(t);
                }

                if (closedChannelException != null) {
                    closedChannelException = new ClosedChannelException();
                }

                notifyFlushFutures(closedChannelException);

                if (wasActive && !isActive()) {
                    pipeline.fireChannelInactive();
                }

                deregister(voidFuture());
            } else {
                // Closed already.
                future.setSuccess();
            }
        } else {
            eventLoop().execute(new Runnable() {
                @Override
                    public void run() {
                        close(future);
                }
            });
        }
    }

    /*
    void flush(final ChannelFuture future) {
        if (eventLoop().inEventLoop()) {
            // Append flush future to the notification list.
            if (future != voidFuture) {
                final int bufSize;
                final ChannelHandlerContext ctx = directOutboundContext();
                if (ctx.hasOutboundByteBuffer()) {
                    bufSize = ctx.outboundByteBuffer().readableBytes();
                } else {
                    bufSize = ctx.outboundMessageBuffer().size();
                }

                long checkpoint = writeCounter + bufSize;
                if (future instanceof FlushCheckpoint) {
                    FlushCheckpoint cp = (FlushCheckpoint) future;
                    cp.flushCheckpoint(checkpoint);
                    flushCheckpoints.add(cp);
                } else {
                    flushCheckpoints.add(new DefaultFlushCheckpoint(checkpoint, future));
                }
            }

            if (!inFlushNow) { // Avoid re-entrance
                try {
                    if (!isFlushPending()) {
                        flushNow();
                    } else {
                        // Event loop will call flushNow() later by itself.
                    }
                } catch (Throwable t) {
                    notifyFlushFutures(t);
                    pipeline.fireExceptionCaught(t);
                    if (t instanceof IOException) {
                        close(voidFuture());
                    }
                } finally {
                    if (!isActive()) {
                        close(unsafe().voidFuture());
                    }
                }
            } else {
                if (!flushNowPending) {
                    flushNowPending = true;
                    eventLoop().execute(flushLaterTask);
                }
            }
        } else {
            eventLoop().execute(new Runnable() {
                @Override
                    public void run() {
                        flush(future);
                }
            });
        }
    }

    void flushNow() {
        if (inFlushNow) {
            return;
        }

        inFlushNow = true;
        ChannelHandlerContext ctx = directOutboundContext();
        Throwable cause = null;
        try {
            if (ctx.hasOutboundByteBuffer()) {
                ByteBuf out = ctx.outboundByteBuffer();
                int oldSize = out.readableBytes();
                try {
                    doFlushByteBuffer(out);
                } catch (Throwable t) {
                    cause = t;
                } finally {
                    final int newSize = out.readableBytes();
                    final int writtenBytes = oldSize - newSize;
                    if (writtenBytes > 0) {
                        writeCounter += writtenBytes;
                        if (newSize == 0) {
                            out.discardReadBytes();
                        }
                    }
                }
            } else {
                MessageBuf<Object> out = ctx.outboundMessageBuffer();
                int oldSize = out.size();
                try {
                    doFlushMessageBuffer(out);
                } catch (Throwable t) {
                    cause = t;
                } finally {
                    writeCounter += oldSize - out.size();
                }
            }

            if (cause == null) {
                notifyFlushFutures();
            } else {
                notifyFlushFutures(cause);
                pipeline.fireExceptionCaught(cause);
                if (cause instanceof IOException) {
                    close(voidFuture());
                }
            }
        } finally {
            inFlushNow = false;
        }
    };*/

protected:
    bool ensureOpen(ChannelFuture future) {
        if (isOpen()) {
            return true;
        }

        Exception e = new ClosedChannelException();
        future.setFailure(e);
        pipeline.fireExceptionCaught(e);
        return false;
    }

    void closeIfClosed() {
        if (isOpen()) {
            return;
        }
        close(voidFuture());
    }

};

}
}

#endif //#if !defined(CETTY_CHANNEL_ABSTRACTCHANNELSINK_H)

// Local Variables:
// mode: c++
// End:

