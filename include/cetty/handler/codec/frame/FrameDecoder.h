#if !defined(CETTY_HANDLER_CODEC_FRAME_FRAMEDECODER_H)
#define CETTY_HANDLER_CODEC_FRAME_FRAMEDECODER_H

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

#include <cetty/buffer/ChannelBufferFwd.h>
#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/SimpleChannelUpstreamHandler.h>

namespace cetty {
    namespace logging {
        class InternalLogger;
    }
}

namespace cetty {
namespace channel {
    class Channel;
    class SocketAddress;
}
}

namespace cetty {
namespace handler {
namespace codec {
namespace frame {

using namespace cetty::channel;
using namespace cetty::buffer;

/**
 * Decodes the received {@link ChannelBuffer}s into a meaningful frame object.
 * <p>
 * In a stream-based transport such as TCP/IP, packets can be fragmented and
 * reassembled during transmission even in a LAN environment.  For example,
 * let us assume you have received three packets:
 * <pre>
 * +-----+-----+-----+
 * | ABC | DEF | GHI |
 * +-----+-----+-----+
 * </pre>
 * because of the packet fragmentation, a server can receive them like the
 * following:
 * <pre>
 * +----+-------+---+---+
 * | AB | CDEFG | H | I |
 * +----+-------+---+---+
 * </pre>
 * <p>
 * {@link FrameDecoder} helps you defrag the received packets into one or more
 * meaningful <strong>frames</strong> that could be easily understood by the
 * application logic.  In case of the example above, your {@link FrameDecoder}
 * implementation could defrag the received packets like the following:
 * <pre>
 * +-----+-----+-----+
 * | ABC | DEF | GHI |
 * +-----+-----+-----+
 * </pre>
 * <p>
 * The following code shows an example handler which decodes a frame whose
 * first 4 bytes header represents the length of the frame, excluding the
 * header.
 * <pre>
 * MESSAGE FORMAT
 * ==============
 *
 * Offset:  0        4                   (Length + 4)
 *          +--------+------------------------+
 * Fields:  | Length | Actual message content |
 *          +--------+------------------------+
 *
 * DECODER IMPLEMENTATION
 * ======================
 *
 * public class IntegerHeaderFrameDecoder extends {@link FrameDecoder} {
 *
 *   protected Object decode({@link ChannelHandlerContext} ctx,
 *                           {@link Channel channel},
 *                           {@link ChannelBuffer} buf) throws Exception {
 *
 *     // Make sure if the length field was received.
 *     if (buf.readableBytes() &lt; 4) {
 *        // The length field was not received yet - return null.
 *        // This method will be invoked again when more packets are
 *        // received and appended to the buffer.
 *        return <strong>null</strong>;
 *     }
 *
 *     // The length field is in the buffer.
 *
 *     // Mark the current buffer position before reading the length field
 *     // because the whole frame might not be in the buffer yet.
 *     // We will reset the buffer position to the marked position if
 *     // there's not enough bytes in the buffer.
 *     buf.markReaderIndex();
 *
 *     // Read the length field.
 *     int length = buf.readInt();
 *
 *     // Make sure if there's enough bytes in the buffer.
 *     if (buf.readableBytes() &lt; length) {
 *        // The whole bytes were not received yet - return null.
 *        // This method will be invoked again when more packets are
 *        // received and appended to the buffer.
 *
 *        // Reset to the marked position to read the length field again
 *        // next time.
 *        buf.resetReaderIndex();
 *
 *        return <strong>null</strong>;
 *     }
 *
 *     // There's enough bytes in the buffer. Read it.
 *     {@link ChannelBuffer} frame = buf.readBytes(length);
 *
 *     // Successfully decoded a frame.  Return the decoded frame.
 *     return <strong>frame</strong>;
 *   }
 * }
 * </pre>
 *
 * <h3>Returning a POJO rather than a {@link ChannelBuffer}</h3>
 * <p>
 * Please note that you can return an object of a different type than
 * {@link ChannelBuffer} in your <tt>decode()</tt> and <tt>code decodeLast()</tt>
 * implementation.  For example, you could return a
 * <a href="http://en.wikipedia.org/wiki/POJO">POJO</a> so that the next
 * {@link ChannelUpstreamHandler} receives a {@link MessageEvent} which
 * contains a POJO rather than a {@link ChannelBuffer}.
 *
 * <h3>Replacing a decoder with another decoder in a pipeline</h3>
 * <p>
 * If you are going to write a protocol multiplexer, you will probably want to
 * replace a {@link FrameDecoder} (protocol detector) with another
 * {@link FrameDecoder} or {@link ReplayingDecoder} (actual protocol decoder).
 * It is not possible to achieve this simply by calling
 * {@link ChannelPipeline#replace(ChannelHandler, String, ChannelHandler)}, but
 * some additional steps are required:
 * <pre>
 * public class FirstDecoder extends {@link FrameDecoder} {
 *
 *     public FirstDecoder() {
 *         super(true); // Enable unfold
 *     }
 *
 *     protected Object decode({@link ChannelHandlerContext} ctx,
 *                             {@link Channel} channel,
 *                             {@link ChannelBuffer} buf) {
 *         ...
 *         // Decode the first message
 *         Object firstMessage = ...;
 *
 *         // Add the second decoder
 *         ctx.getPipeline().addLast("second", new SecondDecoder());
 *
 *         // Remove the first decoder (me)
 *         ctx.getPipeline().remove(this);
 *
 *         if (buf.readable()) {
 *             // Hand off the remaining data to the second decoder
 *             return new Object[] { firstMessage, buf.readBytes(buf.readableBytes()) };
 *         } else {
 *             // Nothing to hand off
 *             return firstMessage;
 *         }
 *     }
 * }
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */

class FrameDecoder : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    virtual void messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e);

    virtual void channelDisconnected(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e);

    virtual void channelClosed(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e);

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
        const ExceptionEvent& e);

protected:
    FrameDecoder();
    FrameDecoder(bool unfold);

    /**
     * Decodes the received packets so far into a frame.
     *
     * @param ctx      the context of this handler
     * @param channel  the current channel
     * @param buffer   the cumulative buffer of received packets so far.
     *                 Note that the buffer might be empty, which means you
     *                 should not make an assumption that the buffer contains
     *                 at least one byte in your decoder implementation.
     *
     * @return the decoded frame if a full frame was received and decoded.
     *         @c null if there's not enough data in the buffer to decode a frame.
     */
    virtual ChannelMessage decode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ChannelBufferPtr& buffer) = 0;

    /**
     * Decodes the received data so far into a frame when the channel is
     * disconnected.
     *
     * @param ctx      the context of this handler
     * @param channel  the current channel
     * @param buffer   the cumulative buffer of received packets so far.
     *                 Note that the buffer might be empty, which means you
     *                 should not make an assumption that the buffer contains
     *                 at least one byte in your decoder implementation.
     *
     * @return the decoded frame if a full frame was received and decoded.
     *         <tt>null</tt> if there's not enough data in the buffer to decode a frame.
     */
    virtual ChannelMessage decodeLast(ChannelHandlerContext& ctx,
                                      const ChannelPtr& channel,
                                      const ChannelBufferPtr& buffer);

private:
    void callDecode(ChannelHandlerContext& context,
        const ChannelPtr& channel,
        const ChannelBufferPtr& buffer,
        const SocketAddress& remoteAddress);

    void unfoldAndFireMessageReceived(ChannelHandlerContext& context,
                                      const SocketAddress& remoteAddress,
                                      ChannelMessage& result);

    void cleanup(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    ChannelBufferPtr& getCumulation(ChannelHandlerContext& ctx);

protected:
    static InternalLogger* logger;

protected:
    bool channelOwnBuffer;
    bool unfold;
    ChannelBufferPtr cumulation;
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_FRAME_FRAMEDECODER_H)

// Local Variables:
// mode: c++
// End:
