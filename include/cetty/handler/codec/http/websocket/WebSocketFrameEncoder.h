#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAMEENCODER_H)
#define CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAMEENCODER_H

/*
 * Copyright 2010 Red Hat, Inc.
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

#include <cetty/handler/codec/oneone/OneToOneEncoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {
namespace websocket {

using namespace cetty::channel;

/**
 * Encodes a {@link WebSocketFrame} into a {@link ChannelBuffer}.
 * <p>
 * For the detailed instruction on adding add Web Socket support to your HTTP
 * server, take a look into the <tt>WebSocketServer</tt> example located in the
 * <tt>org.jboss.netty.example.http.websocket</tt> package.
 *
 *
 * @author Mike Heath (mheath@apache.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.uses org.jboss.netty.handler.codec.http.websocket.WebSocketFrame
 */

class WebSocketFrameEncoder : public cetty::handler::codec::oneone::OneToOneEncoder {
public:
    WebSocketFrameEncoder() {}
    virtual ~WebSocketFrameEncoder() {}

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }
    virtual std::string toString() const { return "WebSocketFrameEncoder"; }

protected:
    virtual ChannelMessage encode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ChannelMessage& msg);
};

}
}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAMEENCODER_H)

// Local Variables:
// mode: c++
// End:
