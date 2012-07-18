#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAME_H)
#define CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAME_H

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

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {
namespace websocket {

using namespace cetty::buffer;

class WebSocketFrame;

/**
 * A Web Socket frame that represents either text or binary data.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

typedef boost::intrusive_ptr<WebSocketFrame> WebSocketFramePtr;

class WebSocketFrame : public cetty::util::ReferenceCounter<WebSocketFrame> {
public:
    virtual ~WebSocketFrame() {}

    /**
     * Closing handshake message (<tt>0xFF, 0x00</tt>)
     */
    static WebSocketFramePtr CLOSING_HANDSHAKE;

    /**
     * Returns the type of this frame.
     * <tt>0x00-0x7F</tt> means a text frame encoded in UTF-8, and
     * <tt>0x80-0xFF</tt> means a binary frame.  Currently, <tt>0</tt> is the
     * only allowed type according to the specification.
     */
    virtual int getType() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the content of this frame is a string
     * encoded in UTF-8.
     */
    virtual bool isText() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the content of this frame is an
     * arbitrary binary data.
     */
    virtual bool isBinary() const = 0;

    /**
     * Returns the content of this frame as-is, with no UTF-8 decoding.
     */
    virtual const ChannelBufferPtr& getBinaryData() const = 0;

    /**
     * Converts the content of this frame into a UTF-8 string and returns the
     * converted string.
     */
    virtual const char* getTextData() const = 0;

    /**
     * Sets the type and the content of this frame.
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
    virtual void setData(int type, const ChannelBufferPtr& binaryData) = 0;

    /**
     * Returns the string representation of this frame.  Please note that this
     * method is not identical to {@link #getTextData()}.
     */
    virtual std::string toString() const = 0;
};

}
}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_WEBSOCKET_WEBSOCKETFRAME_H)

// Local Variables:
// mode: c++
// End:
