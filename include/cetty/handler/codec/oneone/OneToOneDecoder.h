#if !defined(CETTY_HANDLER_CODEC_ONEONE_ONETOONEDECODER_H)
#define CETTY_HANDLER_CODEC_ONEONE_ONETOONEDECODER_H

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
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelUpstreamHandler.h>

namespace cetty {
namespace handler {
namespace codec {
namespace oneone {

using namespace cetty::channel;

/**
 * Transforms a received message into another message.  Please note that this
 * decoder must be used with a proper {@link FrameDecoder} such as
 * {@link DelimiterBasedFrameDecoder} or you must implement proper framing
 * mechanism by yourself if you are using a stream-based transport such as
 * TCP/IP.  A typical setup for TCP/IP would be:
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

class OneToOneDecoder : public ChannelUpstreamHandler {
public:
    virtual ~OneToOneDecoder() {}

    virtual void handleUpstream(ChannelHandlerContext& ctx, const ChannelEvent& evt);

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e);
    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e);
    virtual void writeCompleted(ChannelHandlerContext& ctx, const WriteCompletionEvent& e);
    virtual void channelStateChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e);
    virtual void childChannelStateChanged(ChannelHandlerContext& ctx, const ChildChannelStateEvent& e);

protected:
    /**
     * Transforms the specified received message into another message and return
     * the transformed message.  Return <tt>null</tt> if the received message
     * is supposed to be discarded.
     */
    virtual ChannelMessage decode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ChannelMessage& msg) = 0;

    /**
     * Creates a new instance with the current system character set.
     */
    OneToOneDecoder() {}
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_ONEONE_ONETOONEDECODER_H)

// Local Variables:
// mode: c++
// End:
