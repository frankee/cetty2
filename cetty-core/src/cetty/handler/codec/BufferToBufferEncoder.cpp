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

#include <cetty/handler/codec/BufferToBufferEncoder.h>

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelOutboundBufferHandler.h>
#include <cetty/handler/codec/CodecException.h>
#include <cetty/handler/codec/EncoderException.h>

namespace cetty {
namespace handler {
namespace codec {

BufferToBufferEncoder::BufferToBufferEncoder() {
}

BufferToBufferEncoder::~BufferToBufferEncoder() {
}
#if 0
void BufferToBufferEncoder::flush(ChannelHandlerContext& ctx,
    const ChannelFuturePtr& future) {
        Context* context = ctx.downcast<Context>();

        const ChannelBufferPtr& in = context->getInboundChannelBuffer();
    //ByteBuf out = ctx.nextOutboundByteBuffer();

    int oldOutSize = out.readableBytes();

    while (in->readable()) {
        int oldInSize = in->readableBytes();

        try {
            encode(ctx, in, out);
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

    if (out.readableBytes() > oldOutSize) {
        in->discardReadBytes();
    }

    ctx.flush(future);
}
#endif

}
}
}