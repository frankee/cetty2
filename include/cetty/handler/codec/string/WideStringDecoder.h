#if !defined(CETTY_HANDLER_CODEC_STRING_WIDESTRINGDECODER_H)
#define CETTY_HANDLER_CODEC_STRING_WIDESTRINGDECODER_H


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

#include "cetty/util/Charset.h"
#include "cetty/handler/codec/oneone/OneToOneDecoder.h"

namespace cetty {
namespace handler {
namespace codec {
namespace string {

using namespace cetty::handler::codec::oneone;

/**
 * Decodes a received {@link ChannelBuffer} into a {@link std::string}.  Please
 * note that this decoder must be used with a proper {@link FrameDecoder}
 * such as {@link DelimiterBasedFrameDecoder} if you are using a stream-based
 * transport such as TCP/IP.  A typical setup for a text-based line protocol
 * in a TCP/IP socket would be:
 * <pre>
 * {@link ChannelPipeline} pipeline = ...;
 *
 * // Decoders
 * pipeline.addLast("frameDecoder", new {@link DelimiterBasedFrameDecoder}(80, {@link Delimiters#lineDelimiter()}));
 * pipeline.addLast("stringDecoder", new {@link StringDecoder}(CharsetUtil.UTF_8));
 *
 * // Encoder
 * pipeline.addLast("stringEncoder", new {@link StringEncoder}(CharsetUtil.UTF_8));
 * </pre>
 * and then you can use a {@link std::string} instead of a {@link ChannelBuffer}
 * as a message:
 * <pre>
 * void messageReceived({@link ChannelHandlerContext} ctx, {@link MessageEvent} e) {
 *     std::string msg = (std::string) e.getMessage();
 *     ch.write("Did you say '" + msg + "'?\n");
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

class WideStringDecoder : public cetty::handler::codec::oneone::OneToOneDecoder {
public:
    /**
     * Creates a new instance with the current system character set.
     */
    WideStringDecoder();

    /**
     * Creates a new instance with the specified character set.
     */
    WideStringDecoder(const Charset& charset);

    virtual ~WideStringDecoder() {}

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }

    virtual std::string toString() const { return "WideStringDecoder"; }

protected:
    virtual ChannelMessage decode(
        ChannelHandlerContext& ctx, Channel& channel, const ChannelMessage& msg);

private:
    Charset charset;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_STRING_WIDESTRINGDECODER_H)

// Local Variables:
// mode: c++
// End:

