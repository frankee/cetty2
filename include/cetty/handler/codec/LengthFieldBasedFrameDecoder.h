#if !defined(CETTY_HANDLER_CODEC_LENGTHFIELDBASEDFRAMEDECODER_H)
#define CETTY_HANDLER_CODEC_LENGTHFIELDBASEDFRAMEDECODER_H

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

#include <boost/function.hpp>
#include <cetty/Types.h>

#include <cetty/handler/codec/BufferToMessageDecoder.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;

/**
 * A decoder that splits the received {@link ChannelBuffer}s dynamically by the
 * value of the length field in the message.  It is particularly useful when you
 * decode a binary message which has an integer header field that represents the
 * length of the message body or the whole message.
 * <p>
 * {@link LengthFieldBasedFrameDecoder} has many configuration parameters so
 * that it can decode any message with a length field, which is often seen in
 * proprietary client-server protocols. Here are some example that will give
 * you the basic idea on which option does what.
 *
 * <h3>2 bytes length field at offset 0, do not strip header</h3>
 *
 * The value of the length field in this example is <tt>12 (0x0C)</tt> which
 * represents the length of "HELLO, WORLD".  By default, the decoder assumes
 * that the length field represents the number of the bytes that follows the
 * length field.  Therefore, it can be decoded with the simplistic parameter
 * combination.
 * <pre>
 * <b>lengthFieldOffset</b>   = <b>0</b>
 * <b>lengthFieldLength</b>   = <b>2</b>
 * lengthAdjustment    = 0
 * initialBytesToStrip = 0 (= do not strip header)
 *
 * BEFORE DECODE (14 bytes)         AFTER DECODE (14 bytes)
 * +--------+----------------+----------+       +--------+----------------+
 * | Length | Actual Content | CheckSum | ----->| Length | Actual Content |
 * | 0x000C | "HELLO, WORLD" |          |       | 0x000C | "HELLO, WORLD" |
 * +--------+----------------+----------+       +--------+----------------+
 * </pre>
 *
 * <h3>2 bytes length field at offset 0, strip header</h3>
 *
 * Because we can get the length of the content by calling
 * {@link ChannelBuffer#readableBytes()}, you might want to strip the length
 * field by specifying <tt>initialBytesToStrip</tt>.  In this example, we
 * specified <tt>2</tt>, that is same with the length of the length field, to
 * strip the first two bytes.
 * <pre>
 * lengthFieldOffset   = 0
 * lengthFieldLength   = 2
 * lengthAdjustment    = 0
 * <b>initialBytesToStrip</b> = <b>2</b> (= the length of the Length field)
 *
 * BEFORE DECODE (14 bytes)         AFTER DECODE (12 bytes)
 * +--------+----------------+      +----------------+
 * | Length | Actual Content |----->| Actual Content |
 * | 0x000C | "HELLO, WORLD" |      | "HELLO, WORLD" |
 * +--------+----------------+      +----------------+
 * </pre>
 *
 * <h3>2 bytes length field at offset 0, do not strip header, the length field
 *     represents the length of the whole message</h3>
 *
 * In most cases, the length field represents the length of the message body
 * only, as shown in the previous examples.  However, in some protocols, the
 * length field represents the length of the whole message, including the
 * message header.  In such a case, we specify a non-zero
 * <tt>lengthAdjustment</tt>.  Because the length value in this example message
 * is always greater than the body length by <tt>2</tt>, we specify <tt>-2</tt>
 * as <tt>lengthAdjustment</tt> for compensation.
 * <pre>
 * lengthFieldOffset   =  0
 * lengthFieldLength   =  2
 * <b>lengthAdjustment</b>    = <b>-2</b> (= the length of the Length field)
 * initialBytesToStrip =  0
 *
 * BEFORE DECODE (14 bytes)         AFTER DECODE (14 bytes)
 * +--------+----------------+      +--------+----------------+
 * | Length | Actual Content |----->| Length | Actual Content |
 * | 0x000E | "HELLO, WORLD" |      | 0x000E | "HELLO, WORLD" |
 * +--------+----------------+      +--------+----------------+
 * </pre>
 *
 * <h3>3 bytes length field at the end of 5 bytes header, do not strip header</h3>
 *
 * The following message is a simple variation of the first example.  An extra
 * header value is prepended to the message.  <tt>lengthAdjustment</tt> is zero
 * again because the decoder always takes the length of the prepended data into
 * account during frame length calculation.
 * <pre>
 * <b>lengthFieldOffset</b>   = <b>2</b> (= the length of Header 1)
 * <b>lengthFieldLength</b>   = <b>3</b>
 * lengthAdjustment    = 0
 * initialBytesToStrip = 0
 *
 * BEFORE DECODE (17 bytes)                      AFTER DECODE (17 bytes)
 * +----------+----------+----------------+      +----------+----------+----------------+
 * | Header 1 |  Length  | Actual Content |----->| Header 1 |  Length  | Actual Content |
 * |  0xCAFE  | 0x00000C | "HELLO, WORLD" |      |  0xCAFE  | 0x00000C | "HELLO, WORLD" |
 * +----------+----------+----------------+      +----------+----------+----------------+
 * </pre>
 *
 * <h3>3 bytes length field at the beginning of 5 bytes header, do not strip header</h3>
 *
 * This is an advanced example that shows the case where there is an extra
 * header between the length field and the message body.  You have to specify a
 * positive <tt>lengthAdjustment</tt> so that the decoder counts the extra
 * header into the frame length calculation.
 * <pre>
 * lengthFieldOffset   = 0
 * lengthFieldLength   = 3
 * <b>lengthAdjustment</b>    = <b>2</b> (= the length of Header 1)
 * initialBytesToStrip = 0
 *
 * BEFORE DECODE (17 bytes)                      AFTER DECODE (17 bytes)
 * +----------+----------+----------------+      +----------+----------+----------------+
 * |  Length  | Header 1 | Actual Content |----->|  Length  | Header 1 | Actual Content |
 * | 0x00000C |  0xCAFE  | "HELLO, WORLD" |      | 0x00000C |  0xCAFE  | "HELLO, WORLD" |
 * +----------+----------+----------------+      +----------+----------+----------------+
 * </pre>
 *
 * <h3>2 bytes length field at offset 1 in the middle of 4 bytes header,
 *     strip the first header field and the length field</h3>
 *
 * This is a combination of all the examples above.  There are the prepended
 * header before the length field and the extra header after the length field.
 * The prepended header affects the <tt>lengthFieldOffset</tt> and the extra
 * header affects the <tt>lengthAdjustment</tt>.  We also specified a non-zero
 * <tt>initialBytesToStrip</tt> to strip the length field and the prepended
 * header from the frame.  If you don't want to strip the prepended header, you
 * could specify <tt>0</tt> for <tt>initialBytesToSkip</tt>.
 * <pre>
 * lengthFieldOffset</b>   = 1 (= the length of HDR1)
 * lengthFieldLength</b>   = 2
 * <b>lengthAdjustment</b>    = <b>1</b> (= the length of HDR2)
 * <b>initialBytesToStrip</b> = <b>3</b> (= the length of HDR1 + LEN)
 *
 * BEFORE DECODE (16 bytes)                       AFTER DECODE (13 bytes)
 * +------+--------+------+----------------+      +------+----------------+
 * | HDR1 | Length | HDR2 | Actual Content |----->| HDR2 | Actual Content |
 * | 0xCA | 0x000C | 0xFE | "HELLO, WORLD" |      | 0xFE | "HELLO, WORLD" |
 * +------+--------+------+----------------+      +------+----------------+
 * </pre>
 *
 * <h3>2 bytes length field at offset 1 in the middle of 4 bytes header,
 *     strip the first header field and the length field, the length field
 *     represents the length of the whole message</h3>
 *
 * Let's give another twist to the previous example.  The only difference from
 * the previous example is that the length field represents the length of the
 * whole message instead of the message body, just like the third example.
 * We have to count the length of HDR1 and Length into <tt>lengthAdjustment</tt>.
 * Please note that we don't need to take the length of HDR2 into account
 * because the length field already includes the whole header length.
 * <pre>
 * lengthFieldOffset   =  1
 * lengthFieldLength   =  2
 * <b>lengthAdjustment</b>    = <b>-3</b> (= the length of HDR1 + LEN, negative)
 * <b>initialBytesToStrip</b> = <b> 3</b>
 *
 * BEFORE DECODE (16 bytes)                       AFTER DECODE (13 bytes)
 * +------+--------+------+----------------+      +------+----------------+
 * | HDR1 | Length | HDR2 | Actual Content |----->| HDR2 | Actual Content |
 * | 0xCA | 0x0010 | 0xFE | "HELLO, WORLD" |      | 0xFE | "HELLO, WORLD" |
 * +------+--------+------+----------------+      +------+----------------+
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see LengthFieldPrepender
 */
class LengthFieldBasedFrameDecoder : public BufferToMessageDecoder<ChannelBufferPtr> {
public:
    typedef boost::function2<uint32_t, const uint8_t*, int> ChecksumFunction;

public:
    /**
     * Creates a new instance.
     *
     * @param maxFrameLength
     *        the maximum length of the frame.  If the length of the frame is
     *        greater than this value, {@link TooLongFrameException} will be
     *        thrown.
     * @param lengthFieldOffset
     *        the offset of the length field
     * @param lengthFieldLength
     *        the length of the length field
     *
     */
    LengthFieldBasedFrameDecoder(
        int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength)
        : discardingTooLongFrame(false),
          bytesToDiscard(0),
          maxFrameLength(maxFrameLength),
          lengthFieldOffset(lengthFieldOffset),
          lengthFieldLength(lengthFieldLength),
          lengthFieldEndOffset(lengthFieldOffset + lengthFieldLength),
          lengthAdjustment(0),
          initialBytesToStrip(0),
          checksumFieldLength(0),
          checksumCalcOffset(0) {
        validateParameters();
    }

    LengthFieldBasedFrameDecoder(
        int maxFrameLength,
        int lengthFieldOffset,
        int lengthFieldLength,
        const std::string& header1);



    /**
     * Creates a new instance.
     *
     * @param maxFrameLength
     *        the maximum length of the frame.  If the length of the frame is
     *        greater than this value, {@link TooLongFrameException} will be
     *        thrown.
     * @param lengthFieldOffset
     *        the offset of the length field
     * @param lengthFieldLength
     *        the length of the length field
     * @param lengthAdjustment
     *        the compensation value to add to the value of the length field
     * @param initialBytesToStrip
     *        the number of first bytes to strip out from the decoded frame
     */
    LengthFieldBasedFrameDecoder(int maxFrameLength,
                                 int lengthFieldOffset,
                                 int lengthFieldLength,
                                 int lengthAdjustment,
                                 int initialBytesToStrip)
        : discardingTooLongFrame(false),
          bytesToDiscard(0),
          maxFrameLength(maxFrameLength),
          lengthFieldOffset(lengthFieldOffset),
          lengthFieldLength(lengthFieldLength),
          lengthFieldEndOffset(lengthFieldOffset + lengthFieldLength),
          lengthAdjustment(lengthAdjustment),
          initialBytesToStrip(initialBytesToStrip),
          checksumFieldLength(0),
          checksumCalcOffset(0) {
        validateParameters();
    }

    LengthFieldBasedFrameDecoder(int maxFrameLength,
                                 int lengthFieldOffset,
                                 int lengthFieldLength,
                                 int lengthAdjustment,
                                 int initialBytesToStrip,
                                 const std::string& header1,
                                 const std::string& header2);

    LengthFieldBasedFrameDecoder(int maxFrameLength,
                                 int lengthFieldOffset,
                                 int lengthFieldLength,
                                 int lengthAdjustment,
                                 int initialBytesToStrip,
                                 int checksumFieldLength,
                                 const ChecksumFunction& checksumFunction)
        : discardingTooLongFrame(false),
          bytesToDiscard(0),
          maxFrameLength(maxFrameLength),
          lengthFieldOffset(lengthFieldOffset),
          lengthFieldLength(lengthFieldLength),
          lengthFieldEndOffset(lengthFieldOffset + lengthFieldLength),
          lengthAdjustment(lengthAdjustment),
          initialBytesToStrip(initialBytesToStrip),
          checksumFieldLength(checksumFieldLength),
          checksumCalcOffset(lengthFieldOffset + lengthFieldLength),
          checksumFunction(checksumFunction) {
        validateParameters();
    }

    LengthFieldBasedFrameDecoder(int maxFrameLength,
                                 int lengthFieldOffset,
                                 int lengthFieldLength,
                                 int lengthAdjustment,
                                 int initialBytesToStrip,
                                 int checksumFieldLength,
                                 int checksumCalcOffset,
                                 const ChecksumFunction& checksumFunction)
        : discardingTooLongFrame(false),
          bytesToDiscard(0),
          maxFrameLength(maxFrameLength),
          lengthFieldOffset(lengthFieldOffset),
          lengthFieldLength(lengthFieldLength),
          lengthFieldEndOffset(lengthFieldOffset + lengthFieldLength),
          lengthAdjustment(lengthAdjustment),
          initialBytesToStrip(initialBytesToStrip),
          checksumFieldLength(checksumFieldLength),
          checksumCalcOffset(checksumCalcOffset),
          checksumFunction(checksumFunction) {
        validateParameters();
    }

    LengthFieldBasedFrameDecoder(const LengthFieldBasedFrameDecoder& decoder)
        : discardingTooLongFrame(decoder.discardingTooLongFrame),
          maxFrameLength(decoder.maxFrameLength),
          lengthFieldOffset(decoder.lengthFieldOffset),
          lengthFieldLength(decoder.lengthFieldLength),
          lengthFieldEndOffset(decoder.lengthFieldEndOffset),
          lengthAdjustment(decoder.lengthAdjustment),
          initialBytesToStrip(decoder.initialBytesToStrip),
          checksumFieldLength(decoder.checksumFieldLength),
          checksumCalcOffset(decoder.checksumCalcOffset),
          checksumFunction(decoder.checksumFunction),
          header1(decoder.header1) {
    }

    virtual ~LengthFieldBasedFrameDecoder() {}

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const { return "LengthFieldBasedFrameDecoder"; }

protected:
    virtual ChannelBufferPtr decode(ChannelHandlerContext& ctx,
                                    const ChannelBufferPtr& in);

    /**
     * Extract the sub-region of the specified buffer. This method is called by
     * {@link #decode(ChannelHandlerContext, Channel, ChannelBuffer)} for each
     * frame.  The default implementation returns a copy of the sub-region.
     * For example, you could override this method to use an alternative
     * {@link ChannelBufferFactory}.
     * <p>
     * If you are sure that the frame and its content are not accessed after
     * the current {@link #decode(ChannelHandlerContext, Channel, ChannelBuffer)}
     * call returns, you can even avoid memory copy by returning the sliced
     * sub-region (i.e. <tt>return buffer.slice(index, length)</tt>).
     * It's often useful when you convert the extracted frame into an object.
     * Refer to the source code of {@link ObjectDecoder} to see how this method
     * is overridden to avoid memory copy.
     */
    ChannelBufferPtr extractFrame(const ChannelBufferPtr& buffer, int index, int length);

private:
    void fail(ChannelHandlerContext& ctx, int frameLength);
    void validateParameters();

private:
    bool discardingTooLongFrame;
    int  maxFrameLength;

    int  lengthFieldOffset;
    int  lengthFieldLength;
    int  lengthFieldEndOffset;
    int  lengthAdjustment;

    std::string header1;
    std::string header2;

    int  initialBytesToStrip;

    int  tooLongFrameLength;
    int  bytesToDiscard;

    int  checksumFieldLength;
    int  checksumCalcOffset;
    ChecksumFunction checksumFunction;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_LENGTHFIELDBASEDFRAMEDECODER_H)

// Local Variables:
// mode: c++
// End:
