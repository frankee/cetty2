#if !defined(CETTY_HANDLER_CODEC_CODECUTIL_H)
#define CETTY_HANDLER_CODEC_CODECUTIL_H

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

#include <vector>
#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <cetty/channel/ChannelOutboundBufferHandlerContext.h>
#include <cetty/channel/ChannelInboundMessageHandlerContext.h>
#include <cetty/channel/ChannelOutboundMessageHandlerContext.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;
using namespace cetty::channel;

template<typename T>
class CodecUtil {
public:
    typedef ChannelInboundMessageHandlerContext<T> InboundContext;
    typedef ChannelOutboundMessageHandlerContext<T> OutboundContext;

public:
    static bool unfoldAndAdd(ChannelHandlerContext& ctx, const T& msg, bool inbound) {
        if (!msg) {
            return false;
        }

        if (inbound) {
            InboundContext* nextCtx =
                ctx.nextInboundMessageHandlerContext<InboundContext>();

            if (nextCtx) {
                nextCtx->addInboundMessage(msg);
            }

            return true;
        }
        else {
            OutboundContext* nextCtx =
                ctx.nextInboundMessageHandlerContext<OutboundContext>();

            if (nextCtx) {
                nextCtx->addOutboundMessage(msg);
            }

            return true;
        }

        return false;
    }

private:
    CodecUtil() {}
};

template<typename T>
class CodecUtil<std::vector<T> > {
public:
    static bool unfoldAndAdd(ChannelHandlerContext& ctx, const T& msg, bool inbound) {
        if (msg.empty()) {
            return false;
        }

        bool added = false;

        std::size_t j = msg.size();

        for (std::size_t i = 0; i < j; ++i) {
            if (CodecUtil<T>::unfoldAndAdd(ctx, msg[i], inbound)) {
                added = true;
            }
        }

        return added;
    }

private:
    CodecUtil() {}
};

template<>
class CodecUtil<ChannelBufferPtr> {
public:
    typedef ChannelInboundBufferHandlerContext InboundContext;
    typedef ChannelOutboundBufferHandlerContext OutboundContext;

public:
    static bool unfoldAndAdd(ChannelHandlerContext& ctx, const ChannelBufferPtr& msg, bool inbound) {
        if (!msg) {
            return false;
        }

        if (inbound) {
            InboundContext* nextCtx = ctx.nextInboundBufferHandlerContext();

            if (nextCtx) {
                nextCtx->setInboundChannelBuffer(msg);
            }

            return true;
        }
        else {
            OutboundContext* nextCtx = ctx.nextOutboundBufferHandlerContext();

            if (nextCtx) {
                nextCtx->setOutboundChannelBuffer(msg);
            }

            return true;
        }

        return false;
    }

private:
    CodecUtil() {}
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_CODECUTIL_H)

// Local Variables:
// mode: c++
// End:
