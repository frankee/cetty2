#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERENCODER_H)
#define CETTY_HANDLER_CODEC_BUFFERTOBUFFERENCODER_H
/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/channel/ChannelOutboundBufferHandler.h>
#include <cetty/channel/ChannelPipelineMessageTransfer.h>

namespace cetty {
namespace channel {
class ChannelOutboundBufferHandlerContext;
}
}

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;
using namespace cetty::channel;

template<typename H,
         typename Context = ChannelMessageHandlerContext<H,
         VoidMessage,
         VoidMessage,
         ChannelBufferPtr,
         ChannelBufferPtr,
         VoidMessageContainer,
         VoidMessageContainer,
         ChannelBufferContainer,
         ChannelBufferContainer> >
class BufferToBufferEncoder : boost::noncopyable {
public:
    typedef boost::function<ChannelBufferPtr(ChannelHandlerContext&, const ChannelBufferPtr&)> Encoder;


public:
    BufferToBufferEncoder();
    virtual ~BufferToBufferEncoder();

    void registerTo(Context& ctx) {

    }

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) {
                           const ChannelBufferPtr& in = getOutboundChannelBuffer();
                           ChannelBufferPtr out;
                           //out = ctx.nextOutboundByteBuffer();

                           //int oldOutSize = out->readableBytes();

                           while (in->readable()) {
                               int oldInSize = in->readableBytes();

                               try {
                                   out = encode(ctx, in);
                               }
                               catch (const CodecException& e) {
                                   ctx.fireExceptionCaught(e);
                               }
                               catch (const std::exception& e) {
                                   ctx.fireExceptionCaught(EncoderException(e.what()));
                               }

                               if (oldInSize == in->readableBytes()) {
                                   break;
                               }
                           }

                           //if (out->readableBytes() > oldOutSize) {
                           //    in->discardReadBytes();
                           //}

                           ctx.flush(future);
    }

private:
    Encoder encoder_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERENCODER_H)

// Local Variables:
// mode: c++
// End:
