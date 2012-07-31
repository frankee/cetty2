
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

#include <cetty/channel/socket/asio/AsioSocketChannelSink.h>

#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/socket/asio/AsioSocketChannel.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

AsioSocketChannelSink::AsioSocketChannelSink(AsioSocketChannel& channel)
    : channel(channel), AbstractChannelSink(channel) {
}

void AsioSocketChannelSink::connect(const SocketAddress& remoteAddress,
                                    const SocketAddress& localAddress,
                                    const ChannelFuturePtr& future) {
    if (!ensureOpen(future)) {
        return;
    }

    try {
        //if (connectFuture != null) {
        //    throw new IllegalStateException("connection attempt already made");
        //}

        //connectFuture = future;

        channel.doConnect(remoteAddress, localAddress, future);
#if 0
        // Schedule connect timeout.
        int connectTimeoutMillis = channel->config.getConnectTimeoutMillis();

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

#endif
    }
    catch (const std::exception& t) {
        //future.setFailure(t);
        //pipeline().fireExceptionCaught(t);
        closeIfClosed();
    }
}

#if 0
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
    }
    catch (Throwable t) {
        connectFuture.setFailure(t);
        pipeline().fireExceptionCaught(t);
        closeIfClosed();
    } finally {

        connectTimeoutFuture.cancel(false);
        connectFuture = null;
    }
}
}
#endif

void AsioSocketChannelSink::flush(const ChannelBufferPtr& buffer,
                                  const ChannelFuturePtr& future) {
    channel.doFlush(buffer, future);
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

}
}
}
}