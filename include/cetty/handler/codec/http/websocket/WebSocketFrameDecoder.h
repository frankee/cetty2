#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAMEDECODER_H)
#define CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAMEDECODER_H

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

#include <cetty/handler/codec/replay/ReplayingDecoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {
namespace websocket {

using namespace cetty::channel;
using namespace cetty::handler::codec::replay;

/**
 * Decodes {@link ChannelBuffer}s into {@link WebSocketFrame}s.
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

class WebSocketFrameDecoder : public cetty::handler::codec::replay::ReplayingDecoder {
public:
    static const int DEFAULT_MAX_FRAME_SIZE = 16384;

public:
    WebSocketFrameDecoder()
        : receivedClosingHandshake(true),
          maxFrameSize(DEFAULT_MAX_FRAME_SIZE) {
    }

    /**
     * Creates a new instance of <tt>WebSocketFrameDecoder</tt> with the specified <tt>maxFrameSize</tt>.  If the client
     * sends a frame size larger than <tt>maxFrameSize</tt>, the channel will be closed.
     *
     * @param maxFrameSize  the maximum frame size to decode
     */
    WebSocketFrameDecoder(int maxFrameSize)
        : receivedClosingHandshake(true),
          maxFrameSize(maxFrameSize) {
    }

    virtual ~WebSocketFrameDecoder() {}

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new WebSocketFrameDecoder(maxFrameSize));
    }
    virtual std::string toString() const { return "WebSocketFrameEncoder"; }

protected:
    virtual ChannelMessage decode(ChannelHandlerContext& ctx,
                                  const ChannelPtr& channel,
                                  const ReplayingDecoderBufferPtr& buffer,
                                  int state);

private:
    ChannelMessage decodeBinaryFrame(int type, const ReplayingDecoderBufferPtr& buffer);
    ChannelMessage decodeTextFrame(int type, const ReplayingDecoderBufferPtr& buffer);

private:
    bool receivedClosingHandshake;
    int  maxFrameSize;
};

}
}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAMEDECODER_H)

// Local Variables:
// mode: c++
// End:
