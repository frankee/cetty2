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
#include <cetty/channel/ChannelMessageHandlerContext.h>

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

class LoggingHandler : private boost::noncopyable {
public:
    typedef ChannelMessageHandlerContext<LoggingHandler,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer> Context;

    typedef Context::HandlerPtr Ptr;

public:
    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance with hex dump enabled.
     */
    LoggingHandler()
        : level_(LogLevel::DEBUG) {
    }

    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance.
     *
     * @param level   the log level
     */
    LoggingHandler(const LogLevel& level)
        : level_(level) {
    }

    /**
     * Creates a new instance with the specified logger name.
     */
    LoggingHandler(const std::string& name)
        : level_(LogLevel::DEBUG) {
    }

    /**
     * Creates a new instance with the specified logger name.
     *
     * @param level   the log level
     */
    LoggingHandler(const std::string& name, const LogLevel& level)
        : level_(level) {
    }

    /**
     * Returns the {@link InternalLogLevel} that this handler uses to log
     * a {@link ChannelEvent}.
     */
    const LogLevel& level() {
        return level_;
    }

    void registerTo(Context& ctx) {
        ctx.setChannelOpenCallback(boost::bind(&LoggingHandler::channelOpen,
                                               this,
                                               _1));
        ctx.setChannelActiveCallback(boost::bind(&LoggingHandler::channelActive,
                                     this,
                                     _1));
        ctx.setChannelInactiveCallback(boost::bind(&LoggingHandler::channelInactive,
                                       this,
                                       _1));
        ctx.setExceptionCallback(boost::bind(&LoggingHandler::exceptionCaught,
                                             this,
                                             _1,
                                             _2));
        ctx.setUserEventCallback(boost::bind(&LoggingHandler::userEventTriggered,
                                             this,
                                             _1,
                                             _2));
        ctx.setBindFunctor(boost::bind(&LoggingHandler::bind,
                                       this,
                                       _1,
                                       _2,
                                       _3));
        ctx.setConnectFunctor(boost::bind(&LoggingHandler::connect,
                                          this,
                                          _1,
                                          _2,
                                          _3,
                                          _4));
        ctx.setDisconnectFunctor(boost::bind(&LoggingHandler::disconnect,
                                             this,
                                             _1,
                                             _2));
        ctx.setCloseFunctor(boost::bind(&LoggingHandler::close,
                                        this,
                                        _1,
                                        _2));
    }

private:
    void channelOpen(ChannelHandlerContext& ctx);

    void channelActive(ChannelHandlerContext& ctx);

    void channelInactive(ChannelHandlerContext& ctx);

    void exceptionCaught(ChannelHandlerContext& ctx,
                         const ChannelException& cause);

    void userEventTriggered(ChannelHandlerContext& ctx,
                            const boost::any& evt);

    void bind(ChannelHandlerContext& ctx,
              const InetAddress& localAddress,
              const ChannelFuturePtr& future);

    void connect(ChannelHandlerContext& ctx,
                 const InetAddress& remoteAddress,
                 const InetAddress& localAddress,
                 const ChannelFuturePtr& future);

    void disconnect(ChannelHandlerContext& ctx,
                    const ChannelFuturePtr& future);

    void close(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future);

private:
    LogLevel level_;
    std::string name_;
};

}
}
}


#endif //#if !defined(CETTY_HANDLER_LOGGING_LOGGINGHANDLER_H)

// Local Variables:
// mode: c++
// End:
