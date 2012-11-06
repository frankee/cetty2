#if !defined(CETTY_HANDLER_LOGGING_LOGGINGHANDLER_H)
#define CETTY_HANDLER_LOGGING_LOGGINGHANDLER_H
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
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/logging/LogLevel.h>
#include <cetty/channel/AbstractChannelHandler.h>

namespace cetty {
namespace handler {
namespace logging {

using namespace cetty::logging;
using namespace cetty::channel;

/**
 * A {@link ChannelHandler} that logs all events via {@link InternalLogger}.
 * By default, all events are logged at <tt>DEBUG</tt> level.  You can extend
 * this class and override {@link #log(ChannelEvent)} to change the default
 * behavior.
 * @apiviz.landmark
 */

class LoggingHandler : public cetty::channel::AbstractChannelHandler {
public:
    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance with hex dump enabled.
     */
    LoggingHandler()
        : level(LogLevel::DEBUG) {
    }

    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance.
     *
     * @param level   the log level
     */
    LoggingHandler(const LogLevel& level)
        : level(level) {
    }

    /**
     * Creates a new instance with the specified logger name.
     */
    LoggingHandler(const std::string& name)
        : level(LogLevel::DEBUG) {
    }

    /**
     * Creates a new instance with the specified logger name.
     *
     * @param level   the log level
     */
    LoggingHandler(const std::string& name, const LogLevel& level)
        : level(level) {
    }

    /**
     * Returns the {@link InternalLogLevel} that this handler uses to log
     * a {@link ChannelEvent}.
     */
    const LogLevel& getLevel() {
        return level;
    }

    virtual void channelOpen(ChannelHandlerContext& ctx);

    virtual void channelActive(ChannelHandlerContext& ctx);

    virtual void channelInactive(ChannelHandlerContext& ctx);

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                 const ChannelException& cause);

    virtual void userEventTriggered(ChannelHandlerContext& ctx,
                                    const boost::any& evt);

    virtual void bind(ChannelHandlerContext& ctx,
                      const SocketAddress& localAddress,
                      const ChannelFuturePtr& future);

    virtual void connect(ChannelHandlerContext& ctx,
                         const SocketAddress& remoteAddress,
                         const SocketAddress& localAddress,
                         const ChannelFuturePtr& future);

    virtual void disconnect(ChannelHandlerContext& ctx,
                            const ChannelFuturePtr& future);

    virtual void close(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

    virtual void messageUpdated(ChannelHandlerContext& ctx);

private:
    LogLevel level;
    std::string name;
};

}
}
}


#endif //#if !defined(CETTY_HANDLER_LOGGING_LOGGINGHANDLER_H)

// Local Variables:
// mode: c++
// End:
