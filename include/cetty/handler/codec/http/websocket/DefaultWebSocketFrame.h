#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_DEFAULTWEBSOCKETFRAME_H)
#define CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_DEFAULTWEBSOCKETFRAME_H

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
#include <string>
#include <cetty/handler/codec/http/websocket/WebSocketFrame.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {
namespace websocket {

/**
 * The default {@link WebSocketFrame} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class DefaultWebSocketFrame : public WebSocketFrame {
public:
    /**
     * Creates a new empty text frame.
     */
    DefaultWebSocketFrame();

    /**
     * Creates a new text frame from with the specified string.
     */
    DefaultWebSocketFrame(const std::string& textData);

    DefaultWebSocketFrame(const Array& textData);

    /**
     * Creates a new frame with the specified frame type and the specified data.
     *
     * @param type
     *        the type of the frame. <tt>0</tt> is the only allowed type currently.
     * @param binaryData
     *        the content of the frame.  If <tt>(type &amp; 0x80 == 0)</tt>,
     *        it must be encoded in UTF-8.
     *
     * @throws InvalidArgumentException
     *         if If <tt>(type &amp; 0x80 == 0)</tt> and the data is not encoded
     *         in UTF-8
     */
    DefaultWebSocketFrame(int type, const ChannelBufferPtr& binaryData);

    virtual ~DefaultWebSocketFrame() {}

    virtual int getType() const {
        return type;
    }

    virtual bool isText() const {
        return (type & 0x80) == 0;
    }

    virtual bool isBinary() const {
        return !isText();
    }

    virtual const ChannelBufferPtr& getBinaryData() const {
        return binaryData;
    }

    virtual const char* getTextData() const;
    virtual void setData(int type, const ChannelBufferPtr& binaryData);
    virtual std::string toString() const;

private:
    int type;
    mutable std::string text;
    ChannelBufferPtr binaryData;
};

}
}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_DEFAULTWEBSOCKETFRAME_H)

// Local Variables:
// mode: c++
// End:
