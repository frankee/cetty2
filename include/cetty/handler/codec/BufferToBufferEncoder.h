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

#include <cetty/buffer/ChannelBufferFwd.h>
#include <cetty/channel/ChannelOutboundBufferHandler.h>

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

class BufferToBufferEncoder : public ChannelOutboundBufferHandler {
public:
    typedef ChannelOutboundBufferHandlerContext BufferContext;

public:
    BufferToBufferEncoder();
    virtual ~BufferToBufferEncoder();

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

protected:
    virtual ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                                    const ChannelBufferPtr& in) = 0;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_BUFFERTOBUFFERENCODER_H)

// Local Variables:
// mode: c++
// End:
