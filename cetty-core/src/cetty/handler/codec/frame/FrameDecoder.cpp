/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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
#include <cetty/handler/codec/frame/FrameDecoder.h>

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelEvent.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ExceptionEvent.h>
#include <cetty/channel/ChannelStateEvent.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/buffer/DynamicChannelBuffer.h>
#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace handler {
namespace codec {
namespace frame {

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::util;

InternalLogger* FrameDecoder::logger = NULL;

FrameDecoder::FrameDecoder() : unfold(false) {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("FrameDecoder");
    }
}

FrameDecoder::FrameDecoder(bool unfold) : unfold(unfold) {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("FrameDecoder");
    }
}

void FrameDecoder::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    ChannelBufferPtr input = e.getMessage().smartPointer<ChannelBuffer>();

    if (!input) {
        LOG_WARN(logger, "the received message event is not a channel buffer,\
                         sending to up.");
        ctx.sendUpstream(e);
        return;
    }

    if (!input->readable()) {
        LOG_WARN(logger, "the received message is NOT readable, skip it.");
        return;
    }

    channelOwnBuffer =
        ctx.getChannel()->getConfig().channelHasReaderBuffer();

    if (channelOwnBuffer) {
        callDecode(ctx, e.getChannel(), input, e.getRemoteAddress());
        // if the channel has private ChannelBuffer,
        // then just move the left readable bytes to the begin of
        // the ChannelBuffer.
        // TODO: may be add some strategy to decide whether need to discard or not.
        //       for example, there is a lot capacity to write, and has read a little.
        input->discardReadBytes();
    }
    else {
        ChannelBufferPtr cumulation = getCumulation(ctx);

        if (cumulation->readable()) {
            cumulation->discardReadBytes();
            cumulation->writeBytes(input);
            callDecode(ctx, e.getChannel(), cumulation, e.getRemoteAddress());
        }
        else {
            callDecode(ctx, e.getChannel(), input, e.getRemoteAddress());

            if (input->readable()) {
                cumulation->writeBytes(input);
            }
        }
    }
}

void FrameDecoder::channelDisconnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    cleanup(ctx, e);
}

void FrameDecoder::channelClosed(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    cleanup(ctx, e);
}

void FrameDecoder::exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
    ctx.sendUpstream(e);
}

void FrameDecoder::callDecode(ChannelHandlerContext& context,
                              const ChannelPtr& channel,
                              const ChannelBufferPtr& buffer,
                              const SocketAddress& remoteAddress) {
    while (buffer->readable()) {
        int oldReaderIndex = buffer->readerIndex();
        ChannelMessage frame = decode(context, channel, buffer);

        if (frame.empty()) {
            if (oldReaderIndex == buffer->readerIndex()) {
                // Seems like more data is required.
                // Let us wait for the next notification.
                break;
            }
            else {
                // Previous data has been discarded.
                // Probably it is reading on.
                continue;
            }
        }
        else if (oldReaderIndex == buffer->readerIndex()) {
            const char* msg = "decode() method must read at least one byte \
                              if it returned a frame.";
            LOG_ERROR(logger, msg);
            CETTY_NDC_SCOPE("FrameDecoder::callDecode");
            throw IllegalStateException(msg);
        }

        unfoldAndFireMessageReceived(context, remoteAddress, frame);

        // all read buffer has read, so reset the reader buffer for receiving new buffer.
        if (!buffer->readable()) {
            buffer->clear();
        }
    }
}

void FrameDecoder::unfoldAndFireMessageReceived(ChannelHandlerContext& context,
        const SocketAddress& remoteAddress,
        ChannelMessage&  result) {
    if (unfold && result.isVector()) {
        int i = 0;
        int j = result.vectorSize();

        for (; i < j; ++i) {
            Channels::fireMessageReceived(context,
                                          result.value<ChannelMessage>(i),
                                          remoteAddress);
        }
    }
    else {
        Channels::fireMessageReceived(context, result, remoteAddress);
    }
}

void FrameDecoder::cleanup(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
    try {
        if (!cumulation) {
            return;
        }

        if (cumulation->readable()) {
            // Make sure all frames are read before notifying a closed channel.
            callDecode(ctx, ctx.getChannel(), cumulation, SocketAddress::NULL_ADDRESS);
        }

        // Call decodeLast() finally.  Please note that decodeLast() is
        // called even if there's nothing more to read from the buffer to
        // notify a user that the connection was closed explicitly.
        ChannelMessage partialFrame = decodeLast(ctx, ctx.getChannel(), cumulation);

        if (!partialFrame.empty()) {
            unfoldAndFireMessageReceived(ctx, SocketAddress::NULL_ADDRESS, partialFrame);
        }

        cumulation->clear();
    }
    catch (...) {
        ctx.sendUpstream(e);
    }
}

ChannelBufferPtr& FrameDecoder::getCumulation(ChannelHandlerContext& ctx) {
    if (!cumulation) {
        const ChannelBufferFactoryPtr& factory =
            ctx.getChannel()->getConfig().getBufferFactory();

        BOOST_ASSERT(factory);
        cumulation = ChannelBuffers::dynamicBuffer(*factory);
    }

    return cumulation;
}

cetty::channel::ChannelMessage FrameDecoder::decodeLast(ChannelHandlerContext& ctx,
        const ChannelPtr& channel,
        const ChannelBufferPtr& buffer) {
    return decode(ctx, channel, buffer);
}

}
}
}
}