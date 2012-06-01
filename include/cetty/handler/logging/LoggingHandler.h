#if !defined(CETTY_HANDLER_LOGGING_LOGGINGHANDLER_H)
#define CETTY_HANDLER_LOGGING_LOGGINGHANDLER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/ChannelUpstreamHandler.h>
#include <cetty/channel/ChannelDownstreamHandler.h>

#include <cetty/logging/InternalLogger.h>
#include <cetty/logging/InternalLogLevel.h>
#include <cetty/logging/InternalLoggerFactory.h>

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
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */

class LoggingHandler : public cetty::channel::ChannelDownstreamHandler,
    public cetty::channel::ChannelUpstreamHandler {
public:
    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance with hex dump enabled.
     */
    LoggingHandler() : level(DEFAULT_LEVEL), hexDump(true) {
        logger = InternalLoggerFactory::getInstance("LoggingHandler");
    }

    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance.
     *
     * @param level   the log level
     */
    LoggingHandler(const InternalLogLevel& level)
        : level(level), hexDump(true) {
        logger = InternalLoggerFactory::getInstance("LoggingHandler");
    }

    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance.
     *
     * @param hexDump <tt>true</tt> if and only if the hex dump of the received
     *                message is logged
     */
    LoggingHandler(bool hexDump) : level(DEFAULT_LEVEL), hexDump(hexDump) {
        logger = InternalLoggerFactory::getInstance("LoggingHandler");
    }

    /**
     * Creates a new instance whose logger name is the fully qualified class
     * name of the instance.
     *
     * @param level   the log level
     * @param hexDump <tt>true</tt> if and only if the hex dump of the received
     *                message is logged
     */
    LoggingHandler(const InternalLogLevel& level, bool hexDump)
        : level(level), hexDump(hexDump) {

        logger = InternalLoggerFactory::getInstance("LoggingHandler");
    }

    /**
     * Creates a new instance with the specified logger name and with hex dump
     * enabled.
     */
    LoggingHandler(const std::string& name) : level(DEFAULT_LEVEL), hexDump(true) {
        logger = InternalLoggerFactory::getInstance(name);
    }

    /**
     * Creates a new instance with the specified logger name.
     *
     * @param hexDump <tt>true</tt> if and only if the hex dump of the received
     *                message is logged
     */
    LoggingHandler(const std::string& name, bool hexDump)
        : level(DEFAULT_LEVEL), hexDump(hexDump) {
        logger = InternalLoggerFactory::getInstance(name);
    }

    /**
     * Creates a new instance with the specified logger name.
     *
     * @param level   the log level
     * @param hexDump <tt>true</tt> if and only if the hex dump of the received
     *                message is logged
     */
    LoggingHandler(std::string name, const InternalLogLevel& level, bool hexDump)
        : level(level), hexDump(hexDump) {
        logger = InternalLoggerFactory::getInstance(name);
    }

    /**
     * Returns the {@link InternalLogger} that this handler uses to log
     * a {@link ChannelEvent}.
     */
    InternalLogger& getLogger() {
        return (*logger);
    }

    /**
     * Returns the {@link InternalLogLevel} that this handler uses to log
     * a {@link ChannelEvent}.
     */
    const InternalLogLevel& getLevel() const {
        return level;
    }

    /**
     * Logs the specified event to the {@link InternalLogger} returned by
     * {@link #getLogger()}. If hex dump has been enabled for this handler,
     * the hex dump of the {@link ChannelBuffer} in a {@link MessageEvent} will
     * be logged together.
     */
    void log(const ChannelEvent& e);

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual void handleUpstream(ChannelHandlerContext& ctx, const ChannelEvent& e);
    virtual void handleDownstream(ChannelHandlerContext& ctx, const ChannelEvent& e);

    virtual std::string toString() const { return "LoggingHandler"; }

private:
    static InternalLogLevel DEFAULT_LEVEL;

    InternalLogger* logger;
    InternalLogLevel level;
    bool hexDump;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_LOGGING_LOGGINGHANDLER_H)

// Local Variables:
// mode: c++
// End:

