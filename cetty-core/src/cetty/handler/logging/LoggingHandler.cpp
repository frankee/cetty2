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

#include <cetty/handler/logging/LoggingHandler.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/channel/ExceptionEvent.h>
#include <cetty/channel/ChannelEvent.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace handler {
namespace logging {

using namespace cetty::logging;
using namespace cetty::buffer;
using namespace cetty::channel;

InternalLogLevel LoggingHandler::DEFAULT_LEVEL(InternalLogLevel::DEBUG);

void LoggingHandler::log(const ChannelEvent& e) {
    if (getLogger().isEnabled(level)) {
        std::string msg = e.toString();

        // Append hex dump if necessary.
        if (hexDump) {
            const MessageEvent* me =
                dynamic_cast<const MessageEvent*>(&e);

            if (me) {
                ChannelBufferPtr buf = me->getMessage().smartPointer<ChannelBuffer>();
                msg += " - (HEXDUMP: ";
                msg += ChannelBuffers::hexDump(buf);
                msg += ")";
            }
        }

        // Log the message (and exception if available.)
        const ExceptionEvent* ee =
            dynamic_cast<const ExceptionEvent*>(&e);

        if (ee) {
            getLogger().log(level, msg, ee->getCause());
        }
        else {
            getLogger().log(level, msg);
        }
    }
}

void LoggingHandler::handleUpstream(ChannelHandlerContext& ctx, const ChannelEvent& e) {
    log(e);
    ctx.sendUpstream(e);
}

void LoggingHandler::handleDownstream(ChannelHandlerContext& ctx, const ChannelEvent& e) {
    log(e);
    ctx.sendDownstream(e);
}

}
}
}
