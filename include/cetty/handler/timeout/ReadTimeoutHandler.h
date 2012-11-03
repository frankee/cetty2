#if !defined(CETTY_HANDLER_TIMEOUT_READTIMEOUTHANDLER_H)
#define CETTY_HANDLER_TIMEOUT_READTIMEOUTHANDLER_H

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

#include <boost/function.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/channel/TimeoutPtr.h>
#include <cetty/channel/AbstractChannelInboundHandler.h>
#include <cetty/handler/timeout/ReadTimeoutException.h>


namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;
using namespace cetty::util;

/**
 * Raises a {@link ReadTimeoutException} when no data was read within a certain
 * period of time.
 *
 * <pre>
 * public class MyPipelineFactory implements {@link ChannelPipelineFactory} {
 *
 *     private final {@link Timer} timer;
 *
 *     public MyPipelineFactory({@link Timer} timer) {
 *         this.timer = timer;
 *     }
 *
 *     public {@link ChannelPipeline} getPipeline() {
 *         // An example configuration that implements 30-second read timeout:
 *         return {@link Channels}.pipeline(
 *             <b>new {@link ReadTimeoutHandler}(timer, 30), // timer must be shared.</b>
 *             new MyHandler());
 *     }
 * }
 *
 * {@link ServerBootstrap} bootstrap = ...;
 * {@link Timer} timer = new {@link HashedWheelTimer}();
 * ...
 * bootstrap.setPipelineFactory(new MyPipelineFactory(timer));
 * ...
 * </pre>
 *
 * The {@link Timer} which was specified when the {@link ReadTimeoutHandler} is
 * created should be stopped manually by calling {@link #releaseExternalResources()}
 * or {@link Timer#stop()} when your application shuts down.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see WriteTimeoutHandler
 * @see IdleStateHandler
 *
 * @apiviz.landmark
 * @apiviz.uses org.jboss.netty.util.HashedWheelTimer
 * @apiviz.has org.jboss.netty.handler.timeout.TimeoutException oneway - - raises
 */
class ReadTimeoutHandler : public AbstractChannelInboundHandler {
public:
    typedef boost::posix_time::ptime Time;
    typedef boost::posix_time::time_duration Duration;

private:
    typedef boost::function0<void> TimeoutCallback;

public:
    /**
     * Creates a new instance.
     *
     * @param timer
     *        the {@link Timer} that is used to trigger the scheduled event.
     *        The recommended {@link Timer} implementation is {@link HashedWheelTimer}.
     * @param timeoutSeconds
     *        read timeout in seconds
     */
    ReadTimeoutHandler(int timeoutSeconds);

    /**
     * Creates a new instance.
     *
     * @param timer
     *        the {@link Timer} that is used to trigger the scheduled event.
     *        The recommended {@link Timer} implementation is {@link HashedWheelTimer}.
     * @param timeout
     *        read timeout
     *
     */
    ReadTimeoutHandler(const Duration& timeout);

    virtual void beforeAdd(ChannelHandlerContext& ctx);

    virtual void afterAdd(ChannelHandlerContext& ctx);

    virtual void beforeRemove(ChannelHandlerContext& ctx);

    virtual void afterRemove(ChannelHandlerContext& ctx);

    virtual void channelOpen(ChannelHandlerContext& ctx);

    virtual void channelActive(ChannelHandlerContext& ctx);

    virtual void channelInactive(ChannelHandlerContext& ctx);

    virtual void messageUpdated(ChannelHandlerContext& ctx);

    virtual ChannelHandlerPtr clone();

    virtual std::string toString() const;

protected:
    void readTimedOut(ChannelHandlerContext& ctx);

private:
    void initialize(ChannelHandlerContext& ctx);
    void destroy();

    void handleReadTimeout(ChannelHandlerContext& ctx);

private:
    static const ReadTimeoutException EXCEPTION;

private:
    int state; // 0 - none, 1 - Initialized, 2 - Destroyed;
    bool closed;

    int64_t timeoutMillis;
    Time lastReadTime;

    TimeoutPtr timeout;
    TimeoutCallback timeoutCallback;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_READTIMEOUTHANDLER_H)

// Local Variables:
// mode: c++
// End:
