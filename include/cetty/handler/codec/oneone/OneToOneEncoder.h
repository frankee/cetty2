#if !defined(CETTY_HANDLER_CODEC_ONEONE_ONETOONEENCODER_H)
#define CETTY_HANDLER_CODEC_ONEONE_ONETOONEENCODER_H

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

#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/ChannelDownstreamHandler.h>

namespace cetty {
namespace channel {
class Channel;
class ChannelMessage;
class ChannelHandlerContext;
}
}

namespace cetty {
namespace handler {
namespace codec {
namespace oneone {

using namespace cetty::channel;

/**
 * Transforms a write request into another write request.  A typical setup for
 * TCP/IP would be:
 * <pre>
 * {@link ChannelPipeline} pipeline = ...;
 *
 * // Decoders
 * pipeline.addLast("frameDecoder", new {@link DelimiterBasedFrameDecoder}(80, {@link Delimiters#nulDelimiter()}));
 * pipeline.addLast("customDecoder", new {@link OneToOneDecoder}() { ... });
 *
 * // Encoder
 * pipeline.addLast("customEncoder", new {@link OneToOneEncoder}() { ... });
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */

class OneToOneEncoder : public cetty::channel::ChannelDownstreamHandler {
public:
    virtual ~OneToOneEncoder() {}

    virtual void handleDownstream(ChannelHandlerContext& ctx, const ChannelEvent& evt);

    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e);

    virtual void stateChangeRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

protected:
    /**
     * Transforms the specified message into another message and return the
     * transformed message.  Note that you can not return <tt>null</tt>, unlike
     * you can in {@link OneToOneDecoder#decode(ChannelHandlerContext, Channel, Object)};
     * you must return something, at least {@link ChannelBuffers#EMPTY_BUFFER}.
     */
    virtual ChannelMessage encode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ChannelMessage& msg) = 0;

    OneToOneEncoder() {}
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_ONEONE_ONETOONEENCODER_H)

// Local Variables:
// mode: c++
// End:
