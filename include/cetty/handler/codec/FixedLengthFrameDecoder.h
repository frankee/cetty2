#if !defined(CETTY_HANDLER_CODEC_FIXEDLENGTHFRAMEDECODER_H)
#define CETTY_HANDLER_CODEC_FIXEDLENGTHFRAMEDECODER_H

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

#include <cetty/channel/ChannelHandler.h>
#include <cetty/handler/codec/BufferToMessageDecoder.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;

/**
 * A decoder that splits the received {@link ChannelBuffer}s by the fixed number
 * of bytes. For example, if you received the following four fragmented packets:
 * <pre>
 * +---+----+------+----+
 * | A | BC | DEFG | HI |
 * +---+----+------+----+
 * </pre>
 * A {@link FixedLengthFrameDecoder}<tt>(3)</tt> will decode them into the
 * following three packets with the fixed length:
 * <pre>
 * +-----+-----+-----+
 * | ABC | DEF | GHI |
 * +-----+-----+-----+
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class FixedLengthFrameDecoder : public BufferToMessageDecoder<ChannelBufferPtr> {
public:
    /**
     * Creates a new instance.
     *
     * @param frameLength  the length of the frame
     */
    FixedLengthFrameDecoder(int frameLength) : frameLength(frameLength) {
        if (frameLength <= 0) {
            throw InvalidArgumentException(
                std::string("frameLength must be a positive integer: ") +
                Integer::toString(frameLength));
        }
    }

    virtual ~FixedLengthFrameDecoder() {}

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }
    virtual std::string toString() const { return "FixedLengthFrameDecoder"; }

protected:
    virtual ChannelBufferPtr decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& in) {
        if (in->readableBytes() < frameLength) {
            return UserEvent::EMPTY_EVENT;
        }
        else {
            ChannelBufferPtr sub = buffer->slice(buffer->readerIndex(), frameLength);
            buffer->readerIndex(buffer->readerIndex() + frameLength);

            return UserEvent(sub);
        }
    }

private:
    int frameLength;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_FIXEDLENGTHFRAMEDECODER_H)

// Local Variables:
// mode: c++
// End:
