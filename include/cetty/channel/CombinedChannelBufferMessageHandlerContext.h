#if !defined(CETTY_CHANNEL_COMBINEDCHANNELBUFFERMESSAGEHANDLERCONTEXT_H)
#define CETTY_CHANNEL_COMBINEDCHANNELBUFFERMESSAGEHANDLERCONTEXT_H

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

#include <cetty/channel/ChannelInboundMessageHandlerContext.h>
#include <cetty/channel/ChannelOutboundMessageHandlerContext.h>

namespace cetty {
namespace channel {

template<typename OutboundIn>
class CombinedChannelBufferMessageHandlerContext
    : public ChannelInboundBufferHandlerContext,
      public ChannelOutboundMessageHandlerContext<OutboundIn> {
public:
    CombinedChannelBufferMessageHandlerContext(const std::string& name,
                                 ChannelPipeline& pipeline,
                                 const ChannelHandlerPtr& handler,
                                 ChannelHandlerContext* prev,
                                 ChannelHandlerContext* next)
        : ChannelHandlerContext(name, pipeline, handler, prev, next),
          ChannelInboundBufferHandlerContext(name, pipeline, handler, prev, next),
          ChannelOutboundMessageHandlerContext<OutboundIn>(name, pipeline, handler, prev, next) {
        ChannelOutboundMessageHandlerContext<OutboundIn>::hasOutboundMessageHandler = true;
    }

    CombinedChannelBufferMessageHandlerContext(const std::string& name,
                                 const EventLoopPtr& eventLoop,
                                 ChannelPipeline& pipeline,
                                 const ChannelHandlerPtr& handler,
                                 ChannelHandlerContext* prev,
                                 ChannelHandlerContext* next)
        : ChannelHandlerContext(name, eventLoop, pipeline, handler, prev, next),
          ChannelInboundBufferHandlerContext(name, eventLoop, pipeline, handler, prev, next),
          ChannelOutboundMessageHandlerContext<OutboundIn>(name, eventLoop, pipeline, handler, prev, next) {
        ChannelOutboundMessageHandlerContext<OutboundIn>::hasOutboundMessageHandler = true;
    }

    virtual ~CombinedChannelBufferMessageHandlerContext() {}

};

}
}

#endif //#if !defined(CETTY_CHANNEL_COMBINEDCHANNELBUFFERMESSAGEHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:
