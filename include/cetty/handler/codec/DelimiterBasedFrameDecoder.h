#if !defined(CETTY_HANDLER_CODEC_FRAME_DELIMITERBASEDFRAMEDECODER_H)
#define CETTY_HANDLER_CODEC_FRAME_DELIMITERBASEDFRAMEDECODER_H

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

#include <cetty/handler/codec/BufferToMessageDecoder.h>

namespace cetty {
namespace handler {
namespace codec {

/**
 * A decoder that splits the received {@link ChannelBuffer}s by one or more
 * delimiters.  It is particularly useful for decoding the frames which ends
 * with a delimiter such as {@link Delimiters#nulDelimiter() NUL} or
 * @link Delimiters#lineDelimiter() newline characters@endlink.
 *
 * <h3>Predefined delimiters</h3>
 * <p>
 * {@link Delimiters} defines frequently used delimiters for convenience' sake.
 *
 * <h3>Specifying more than one delimiter</h3>
 * <p>
 * {@link DelimiterBasedFrameDecoder} allows you to specify more than one
 * delimiter.  If more than one delimiter is found in the buffer, it chooses
 * the delimiter which produces the shortest frame.  For example, if you have
 * the following data in the buffer:
 * <pre>
 * +--------------+
 * | ABC\nDEF\r\n |
 * +--------------+
 * </pre>
 * a {@link DelimiterBasedFrameDecoder}({@link Delimiters#lineDelimiter() Delimiters.lineDelimiter()})
 * will choose <tt>'\n'</tt> as the first delimiter and produce two frames:
 * <pre>
 * +-----+-----+
 * | ABC | DEF |
 * +-----+-----+
 * </pre>
 * rather than incorrectly choosing <tt>'\r\n'</tt> as the first delimiter:
 * <pre>
 * +----------+
 * | ABC\nDEF |
 * +----------+
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.uses org.jboss.netty.handler.codec.frame.Delimiters - - useful
 */

class DelimiterBasedFrameDecoder : public BufferToMessageDecoder<ChannelBufferPtr> {
public:
    /**
     * Creates a new instance.
     *
     * @param maxFrameLength  the maximum length of the decoded frame.
     *                        A {@link TooLongFrameException} is thrown if
     *                        the length of the frame exceeds this value.
     * @param delimiter  the delimiter
     */
    DelimiterBasedFrameDecoder(int maxFrameLength,
        const ChannelBufferPtr& delimiter);

    /**
     * Creates a new instance.
     *
     * @param maxFrameLength  the maximum length of the decoded frame.
     *                        A {@link TooLongFrameException} is thrown if
     *                        the length of the frame exceeds this value.
     * @param stripDelimiter  whether the decoded frame should strip out the
     *                        delimiter or not
     * @param delimiter  the delimiter
     */
    DelimiterBasedFrameDecoder(int maxFrameLength,
        bool stripDelimiter,
        const ChannelBufferPtr& delimiter);

    /**
     * Creates a new instance.
     *
     * @param maxFrameLength  the maximum length of the decoded frame.
     *                        A {@link TooLongFrameException} is thrown if
     *                        the length of the frame exceeds this value.
     * @param delimiters  the delimiters
     */
    DelimiterBasedFrameDecoder(int maxFrameLength,
        const std::vector<ChannelBufferPtr>& delimiters);

    /**
     * Creates a new instance.
     *
     * @param maxFrameLength  the maximum length of the decoded frame.
     *                        A {@link TooLongFrameException} is thrown if
     *                        the length of the frame exceeds this value.
     * @param stripDelimiter  whether the decoded frame should strip out the
     *                        delimiter or not
     * @param delimiters  the delimiters
     */
    DelimiterBasedFrameDecoder(int maxFrameLength,
        bool stripDelimiter,
        const std::vector<ChannelBufferPtr>& delimiters);

    virtual ~DelimiterBasedFrameDecoder() {}

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new DelimiterBasedFrameDecoder(maxFrameLength,
                                 stripDelimiter,
                                 delimiters));
    }
    virtual std::string toString() const { return "FixedLengthFrameDecoder"; }

protected:
    virtual ChannelBufferPtr decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& in);

    virtual UserEvent decode(
        ChannelHandlerContext& ctx, const ChannelPtr& channel, const ChannelBufferPtr& buffer);

private:
    void fail(ChannelHandlerContext& ctx, long frameLength);
    void initdelimiters(const std::vector<ChannelBufferPtr>& delimiters);

    /**
     * Returns the number of bytes between the readerIndex of the haystack and
     * the first needle found in the haystack.  -1 is returned if no needle is
     * found in the haystack.
     */
    static int indexOf(const ChannelBufferPtr& haystack, const ChannelBufferPtr& needle);

    static void validateDelimiter(const ChannelBufferPtr& delimiter);
    static void validateMaxFrameLength(int maxFrameLength);

private:
    std::vector<ChannelBufferPtr> delimiters;
    int maxFrameLength;
    bool stripDelimiter;

    bool discardingTooLongFrame;
    int tooLongFrameLength;
};


}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_FRAME_DELIMITERBASEDFRAMEDECODER_H)

// Local Variables:
// mode: c++
// End:

