#if !defined(CETTY_HANDLER_CODEC_REPLAY_REPLAYINGDECODER_H)
#define CETTY_HANDLER_CODEC_REPLAY_REPLAYINGDECODER_H

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

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/SimpleChannelUpstreamHandler.h>
#include <cetty/handler/codec/replay/ReplayingDecoderBuffer.h>

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
namespace replay {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::logging;

/**
 * A specialized variation of {@link FrameDecoder} which enables implementation
 * of a non-blocking decoder in the blocking I/O paradigm.
 * <p>
 * The biggest difference between {@link ReplayingDecoder} and
 * {@link FrameDecoder} is that {@link ReplayingDecoder} allows you to
 * implement the <tt>decode()</tt> and <tt>decodeLast()</tt> methods just like
 * all required bytes were received already, rather than checking the
 * availability of the required bytes.  For example, the following
 * {@link FrameDecoder} implementation:
 * <pre>
 * public class IntegerHeaderFrameDecoder extends {@link FrameDecoder} {
 *
 *   protected Object decode({@link ChannelHandlerContext} ctx,
 *                           {@link Channel} channel,
 *                           {@link ChannelBuffer} buf) throws Exception {
 *
 *     if (buf.readableBytes() &lt; 4) {
 *        return <strong>null</strong>;
 *     }
 *
 *     buf.markReaderIndex();
 *     int length = buf.readInt();
 *
 *     if (buf.readableBytes() &lt; length) {
 *        buf.resetReaderIndex();
 *        return <strong>null</strong>;
 *     }
 *
 *     return buf.readBytes(length);
 *   }
 * }
 * </pre>
 * is simplified like the following with {@link ReplayingDecoder}:
 * <pre>
 * public class IntegerHeaderFrameDecoder
 *      extends {@link ReplayingDecoder}&lt;{@link VoidEnum}&gt; {
 *
 *   protected Object decode({@link ChannelHandlerContext} ctx,
 *                           {@link Channel} channel,
 *                           {@link ChannelBuffer} buf,
 *                           {@link VoidEnum} state) throws Exception {
 *
 *     return buf.readBytes(buf.readInt());
 *   }
 * }
 * </pre>
 *
 * <h3>How does this work?</h3>
 * <p>
 * {@link ReplayingDecoder} passes a specialized {@link ChannelBuffer}
 * implementation which throws an {@link Error} of certain type when there's not
 * enough data in the buffer.  In the <tt>IntegerHeaderFrameDecoder</tt> above,
 * you just assumed that there will be 4 or more bytes in the buffer when
 * you call <tt>buf.readInt()</tt>.  If there's really 4 bytes in the buffer,
 * it will return the integer header as you expected.  Otherwise, the
 * {@link Error} will be raised and the control will be returned to
 * {@link ReplayingDecoder}.  If {@link ReplayingDecoder} catches the
 * {@link Error}, then it will rewind the <tt>readerIndex</tt> of the buffer
 * back to the 'initial' position (i.e. the beginning of the buffer) and call
 * the <tt>decode(..)</tt> method again when more data is received into the
 * buffer.
 * <p>
 * Please note that {@link ReplayingDecoder} always throws the same cached
 * {@link Error} instance to avoid the overhead of creating a new {@link Error}
 * and filling its stack trace for every throw.
 *
 * <h3>Limitations</h3>
 * <p>
 * At the cost of the simplicity, {@link ReplayingDecoder} enforces you a few
 * limitations:
 * <ul>
 * <li>Some buffer operations are prohibited.</li>
 * <li>Performance can be worse if the network is slow and the message
 *     format is complicated unlike the example above.  In this case, your
 *     decoder might have to decode the same part of the message over and over
 *     again.</li>
 * <li>You must keep in mind that <tt>decode(..)</tt> method can be called many
 *     times to decode a single message.  For example, the following code will
 *     not work:
 * <pre> public class MyDecoder extends {@link ReplayingDecoder}&lt;{@link VoidEnum}&gt; {
 *
 *   private final Queue&lt;Integer&gt; values = new LinkedList&lt;Integer&gt;();
 *
 *   public Object decode(.., {@link ChannelBuffer} buffer, ..) throws Exception {
 *
 *     // A message contains 2 integers.
 *     values.offer(buffer.readInt());
 *     values.offer(buffer.readInt());
 *
 *     // This assertion will fail intermittently since values.offer()
 *     // can be called more than two times!
 *     assert values.size() == 2;
 *     return values.poll() + values.poll();
 *   }
 * }</pre>
 *      The correct implementation looks like the following, and you can also
 *      utilize the 'checkpoint' feature which is explained in detail in the
 *      next section.
 * <pre> public class MyDecoder extends {@link ReplayingDecoder}&lt;{@link VoidEnum}&gt; {
 *
 *   private final Queue&lt;Integer&gt; values = new LinkedList&lt;Integer&gt;();
 *
 *   public Object decode(.., {@link ChannelBuffer} buffer, ..) throws Exception {
 *
 *     // Revert the state of the variable that might have been changed
 *     // since the last partial decode.
 *     values.clear();
 *
 *     // A message contains 2 integers.
 *     values.offer(buffer.readInt());
 *     values.offer(buffer.readInt());
 *
 *     // Now we know this assertion will never fail.
 *     assert values.size() == 2;
 *     return values.poll() + values.poll();
 *   }
 * }</pre>
 *     </li>
 * </ul>
 *
 * <h3>Improving the performance</h3>
 * <p>
 * Fortunately, the performance of a complex decoder implementation can be
 * improved significantly with the <tt>checkpoint()</tt> method.  The
 * <tt>checkpoint()</tt> method updates the 'initial' position of the buffer so
 * that {@link ReplayingDecoder} rewinds the <tt>readerIndex</tt> of the buffer
 * to the last position where you called the <tt>checkpoint()</tt> method.
 *
 * <h4>Calling <tt>checkpoint(T)</tt> with an {@link Enum}</h4>
 * <p>
 * Although you can just use <tt>checkpoint()</tt> method and manage the state
 * of the decoder by yourself, the easiest way to manage the state of the
 * decoder is to create an {@link Enum} type which represents the current state
 * of the decoder and to call <tt>checkpoint(T)</tt> method whenever the state
 * changes.  You can have as many states as you want depending on the
 * complexity of the message you want to decode:
 *
 * <pre>
 * enum MyDecoderState {
 *   READ_LENGTH,
 *   READ_CONTENT;
 * }
 *
 * class IntegerHeaderFrameDecoder
 *      extends {@link ReplayingDecoder}&lt;<strong>MyDecoderState</strong>&gt; {
 *
 *   private int length;
 *
 *   public IntegerHeaderFrameDecoder() {
 *     // Set the initial state.
 *     <strong>super(MyDecoderState.READ_LENGTH);</strong>
 *   }
 *
 *   protected Object decode({@link ChannelHandlerContext} ctx,
 *                           {@link Channel} channel,
 *                           {@link ChannelBuffer} buf,
 *                           <b>MyDecoderState</b> state) throws Exception {
 *     switch (state) {
 *     case READ_LENGTH:
 *       length = buf.readInt();
 *       <strong>checkpoint(MyDecoderState.READ_CONTENT);</strong>
 *     case READ_CONTENT:
 *       ChannelBuffer frame = buf.readBytes(length);
 *       <strong>checkpoint(MyDecoderState.READ_LENGTH);</strong>
 *       return frame;
 *     default:
 *       throw new Error("Shouldn't reach here.");
 *     }
 *   }
 * }
 * </pre>
 *
 * <h4>Calling <tt>checkpoint()</tt> with no parameter</h4>
 * <p>
 * An alternative way to manage the decoder state is to manage it by yourself.
 * <pre>
 * public class IntegerHeaderFrameDecoder
 *      extends {@link ReplayingDecoder}&lt;<strong>{@link VoidEnum}</strong>&gt; {
 *
 *   <strong>private bool readLength;</strong>
 *   private int length;
 *
 *   <tt>@Override</tt>
 *   protected Object decode({@link ChannelHandlerContext} ctx,
 *                           {@link Channel} channel,
 *                           {@link ChannelBuffer} buf,
 *                           {@link VoidEnum} state) throws Exception {
 *     if (!readLength) {
 *       length = buf.readInt();
 *       <strong>readLength = true;</strong>
 *       <strong>checkpoint();</strong>
 *     }
 *
 *     if (readLength) {
 *       ChannelBuffer frame = buf.readBytes(length);
 *       <strong>readLength = false;</strong>
 *       <strong>checkpoint();</strong>
 *       return frame;
 *     }
 *   }
 * }
 * </pre>
 *
 * <h3>Replacing a decoder with another decoder in a pipeline</h3>
 * <p>
 * If you are going to write a protocol multiplexer, you will probably want to
 * replace a {@link ReplayingDecoder} (protocol detector) with another
 * {@link ReplayingDecoder} or {@link FrameDecoder} (actual protocol decoder).
 * It is not possible to achieve this simply by calling
 * {@link ChannelPipeline#replace(ChannelHandler, std::string, ChannelHandler)}, but
 * some additional steps are required:
 * <pre>
 * public class FirstDecoder extends {@link ReplayingDecoder}&lt;{@link VoidEnum}&gt; {
 *
 *     public FirstDecoder() {
 *         super(true); // Enable unfold
 *     }
 *
 *     protected Object decode({@link ChannelHandlerContext} ctx,
 *                             {@link Channel} ch,
 *                             {@link ChannelBuffer} buf,
 *                             {@link VoidEnum} state) {
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
 *             return new Object[] { firstMessage, buf.readBytes(<b>super.actualReadableBytes()</b>) };
 *         } else {
 *             // Nothing to hand off
 *             return firstMessage;
 *         }
 *     }
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @param <T>
 *        the state type; use {@link VoidEnum} if state management is unused
 *
 * @apiviz.landmark
 * @apiviz.has org.jboss.netty.handler.codec.replay.UnreplayableOperationException oneway - - throws
 */

class ReplayingDecoder : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    virtual ~ReplayingDecoder() {}

    virtual void messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e);

    virtual void channelDisconnected(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e);

    virtual void channelClosed(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e);

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
        const ExceptionEvent& e);

protected:
    /**
     * Creates a new instance with no initial state (i.e: <tt>null</tt>).
     */
    ReplayingDecoder()
        : channelOwnBuffer(false), unfold(false), state(-1), checkedPoint(-1) {
    }

    ReplayingDecoder(bool unfold)
        : channelOwnBuffer(false), unfold(unfold), state(-1), checkedPoint(-1) {
    }

    /**
     * Creates a new instance with the specified initial state.
     */
    ReplayingDecoder(int initialState)
        : channelOwnBuffer(false), unfold(false), state(initialState), checkedPoint(-1) {
    }

    ReplayingDecoder(int initialState, bool unfold)
        : channelOwnBuffer(false), unfold(unfold), state(initialState), checkedPoint(-1) {
    }

    /**
     * Stores the internal cumulative buffer's reader position.
     */
    void checkpoint() {
        if (cumulation) {
            checkedPoint = cumulation->readerIndex();
        }
        else {
            checkedPoint = -1; // buffer not available (already cleaned up)
        }
    }

    /**
     * Stores the internal cumulative buffer's reader position and updates
     * the current decoder state.
     */
    void checkpoint(int state) {
        checkpoint();
        setState(state);
    }

    /**
     * Returns the current state of this decoder.
     * @return the current state of this decoder
     */
    int getState() const {
        return state;
    }

    /**
     * Sets the current state of this decoder.
     * @return the old state of this decoder
     */
    int setState(int newState) {
        int oldState = state;
        state = newState;
        return oldState;
    }

    /**
     * Returns the actual number of readable bytes in the internal cumulative
     * buffer of this decoder.  You usually do not need to rely on this value
     * to write a decoder.  Use it only when you muse use it at your own risk.
     * This method is a shortcut to {@link #internalBuffer() internalBuffer().readableBytes()}.
     */
    int actualReadableBytes() const {
        return internalBuffer()->readableBytes();
    }

    /**
     * Returns the internal cumulative buffer of this decoder.  You usually
     * do not need to access the internal buffer directly to write a decoder.
     * Use it only when you must use it at your own risk.
     */
    const ChannelBufferPtr& internalBuffer() const;

    /**
     * Decodes the received packets so far into a frame.
     *
     * @param ctx      the context of this handler
     * @param channel  the current channel
     * @param buffer   the cumulative buffer of received packets so far.
     *                 Note that the buffer might be empty, which means you
     *                 should not make an assumption that the buffer contains
     *                 at least one byte in your decoder implementation.
     * @param state    the current decoder state (<tt>null</tt> if unused)
     *
     * @return the decoded frame
     */
    virtual ChannelMessage decode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ReplayingDecoderBufferPtr& buffer,
                                  int state) = 0;

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
     * @param state    the current decoder state (<tt>null</tt> if unused)
     *
     * @return the decoded frame
     */
    virtual ChannelMessage decodeLast(ChannelHandlerContext& ctx,
                                      const ChannelPtr& channel,
                                      const ReplayingDecoderBufferPtr& buffer,
                                      int state) {
        return decode(ctx, channel, buffer, state);
    }

private:
    void callDecode(ChannelHandlerContext& context,
                    const ChannelPtr& channel,
                    const ChannelBufferPtr& cumulation,
                    const SocketAddress& remoteAddress);

    void unfoldAndfireMessageReceived(ChannelHandlerContext& context,
                                      ChannelMessage& result,
                                      const SocketAddress& remoteAddress);

    void cleanup(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    const ChannelBufferPtr& getCumulation(ChannelHandlerContext& ctx, const ChannelBufferPtr& input);

private:
    static InternalLogger* logger;

private:
    bool channelOwnBuffer;
    bool unfold;
    int  state;
    int  checkedPoint;

    ChannelBufferPtr cumulation;
    ReplayingDecoderBufferPtr replayable;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_REPLAY_REPLAYINGDECODER_H)

// Local Variables:
// mode: c++
// End:

