#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERDECODER_H)
#define CETTY_HANDLER_CODEC_BUFFERTOBUFFERDECODER_H
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

#include <cetty/channel/ChannelInboundBufferHandler.h>
#include <cetty/channel/ChannelPipelineMessageTransfer.h>
#include <cetty/buffer/ChannelBufferPtr.h>

namespace cetty {
namespace channel {
class ChannelHandlerContext;
class ChannelInboundBufferHandlerContext;
}
}

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;
using namespace cetty::buffer;

class BufferToBufferDecoder : public cetty::channel::ChannelInboundBufferHandler {
public:
    typedef ChannelInboundBufferHandlerContext BufferContext;

public:
    BufferToBufferDecoder();
    virtual ~BufferToBufferDecoder();

    virtual void channelInactive(ChannelHandlerContext& ctx);

    ChannelBufferPtr decodeLast(ChannelHandlerContext& ctx,
                                const ChannelBufferPtr& in) {
        return decode(ctx, in);
    }

    virtual ChannelBufferPtr decode(ChannelHandlerContext& ctx,
                                    const ChannelBufferPtr& in) = 0;

protected:
    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const ChannelBufferPtr& in);

private:
    ChannelBufferPtr callDecode(ChannelHandlerContext& ctx,
                                const ChannelBufferPtr& in);

protected:
    ChannelPipelineMessageTransfer<ChannelBufferPtr, BufferContext> inboundTransfer;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERDECODER_H)

// Local Variables:
// mode: c++
// End:
