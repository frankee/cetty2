#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEHANDLER_H)
#define CETTY_HANDLER_TIMEOUT_IDLESTATEHANDLER_H

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

#include <cetty/Types.h>
#include <cetty/channel/TimeoutPtr.h>
#include <cetty/channel/AbstractChannelHandler.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;
using namespace cetty::util;

class IdleState;
class IdleStateEvent;

/**
 * Triggers an {@link IdleStateEvent} when a {@link Channel} has not performed
 * read, write, or both operation for a while.
 *
 * <h3>Supported idle states</h3>
 * <table border="1">
 * <tr>
 * <th>Property</th><th>Meaning</th>
 * </tr>
 * <tr>
 * <td><tt>readerIdleTime</tt></td>
 * <td>an {@link IdleStateEvent} whose state is {@link IdleState#READER_IDLE}
 *     will be triggered when no read was performed for the specified period of
 *     time.  Specify <tt>0</tt> to disable.</td>
 * </tr>
 * <tr>
 * <td><tt>writerIdleTime</tt></td>
 * <td>an {@link IdleStateEvent} whose state is {@link IdleState#WRITER_IDLE}
 *     will be triggered when no write was performed for the specified period of
 *     time.  Specify <tt>0</tt> to disable.</td>
 * </tr>
 * <tr>
 * <td><tt>allIdleTime</tt></td>
 * <td>an {@link IdleStateEvent} whose state is {@link IdleState#ALL_IDLE}
 *     will be triggered when neither read nor write was performed for the
 *     specified period of time.  Specify <tt>0</tt> to disable.</td>
 * </tr>
 * </table>
 *
 * <pre>
 * // An example that sends a ping message when there is no outbound traffic
 * // for 30 seconds.  The connection is closed when there is no inbound traffic
 * // for 60 seconds.
 *
 * public class MyPipelineFactory implements {@link ChannelPipelineFactory} {
 *
 *     private final {@link Timer} timer;
 *
 *     public MyPipelineFactory({@link Timer} timer) {
 *         this.timer = timer;
 *     }
 *
 *     public {@link ChannelPipeline} getPipeline() {
 *         return {@link Channels}.pipeline(
 *             <b>new {@link IdleStateHandler}(timer, 60, 30, 0), // timer must be shared.</b>
 *             new MyHandler());
 *     }
 * }
 *
 * // Handler should handle the {@link IdleStateEvent} triggered by {@link IdleStateHandler}.
 * public class MyHandler extends {@link IdleStateAwareChannelHandler} {
 *
 *     <tt>@Override</tt>
 *     public void channelIdle({@link ChannelHandlerContext} ctx, {@link IdleStateEvent} e) {
 *         if (e.getState() == {@link IdleState}.READER_IDLE) {
 *             e.getChannel().close();
 *         } else if (e.getState() == {@link IdleState}.WRITER_IDLE) {
 *             e.getChannel().write(new PingMessage());
 *         }
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
 * @see ReadTimeoutHandler
 * @see WriteTimeoutHandler
 *
 * @apiviz.landmark
 * @apiviz.uses org.jboss.netty.util.HashedWheelTimer
 * @apiviz.has org.jboss.netty.handler.timeout.IdleStateEvent oneway - - triggers
 */

class IdleStateHandler : public cetty::channel::AbstractChannelHandler {
public:
    typedef boost::posix_time::ptime Time;
    typedef boost::posix_time::time_duration Duration;

    typedef boost::function2<void, ChannelHandlerContext&, const IdleStateEvent&> IdleEventCallback;

private:
    typedef boost::function0<void> TimeoutCallback;

public:
    /**
     * Creates a new instance.
     *
     * @param timer
     *        the {@link Timer} that is used to trigger the scheduled event.
     *        The recommended {@link Timer} implementation is {@link HashedWheelTimer}.
     * @param readerIdleTimeSeconds
     *        an {@link IdleStateEvent} whose state is {@link IdleState#READER_IDLE}
     *        will be triggered when no read was performed for the specified
     *        period of time.  Specify <tt>0</tt> to disable.
     * @param writerIdleTimeSeconds
     *        an {@link IdleStateEvent} whose state is {@link IdleState#WRITER_IDLE}
     *        will be triggered when no write was performed for the specified
     *        period of time.  Specify <tt>0</tt> to disable.
     * @param allIdleTimeSeconds
     *        an {@link IdleStateEvent} whose state is {@link IdleState#ALL_IDLE}
     *        will be triggered when neither read nor write was performed for
     *        the specified period of time.  Specify <tt>0</tt> to disable.
     */
    IdleStateHandler(int readerIdleTimeSeconds,
                     int writerIdleTimeSeconds,
                     int allIdleTimeSeconds);

    /**
     * Creates a new instance.
     *
     * @param timer
     *        the {@link Timer} that is used to trigger the scheduled event.
     *        The recommended {@link Timer} implementation is {@link HashedWheelTimer}.
     * @param readerIdleTime
     *        an {@link IdleStateEvent} whose state is {@link IdleState#READER_IDLE}
     *        will be triggered when no read was performed for the specified
     *        period of time.  Specify <tt>0</tt> to disable.
     * @param writerIdleTime
     *        an {@link IdleStateEvent} whose state is {@link IdleState#WRITER_IDLE}
     *        will be triggered when no write was performed for the specified
     *        period of time.  Specify <tt>0</tt> to disable.
     * @param allIdleTime
     *        an {@link IdleStateEvent} whose state is {@link IdleState#ALL_IDLE}
     *        will be triggered when neither read nor write was performed for
     *        the specified period of time.  Specify <tt>0</tt> to disable.
     * @param unit
     *        the {@link TimeUnit} of <tt>readerIdleTime</tt>,
     *        <tt>writeIdleTime</tt>, and <tt>allIdleTime</tt>
     */
    IdleStateHandler(const Duration& readerIdleTime,
                     const Duration& writerIdleTime,
                     const Duration& allIdleTime);

    /**
     * Return the readerIdleTime that was given when instance this class in milliseconds.
     *
     */
    int64_t getReaderIdleTimeInMillis() const {
        return readerIdleTimeMillis;
    }

    /**
     * Return the writerIdleTime that was given when instance this class in milliseconds.
     *
     */
    int64_t getWriterIdleTimeInMillis() const {
        return writerIdleTimeMillis;
    }

    /**
     * Return the allIdleTime that was given when instance this class in milliseconds.
     *
     */
    int64_t getAllIdleTimeInMillis() const {
        return allIdleTimeMillis;
    }

    virtual void beforeAdd(ChannelHandlerContext& ctx);
    virtual void afterAdd(ChannelHandlerContext& ctx); // NOOP

    virtual void beforeRemove(ChannelHandlerContext& ctx);
    virtual void afterRemove(ChannelHandlerContext& ctx); // NOOP

    virtual void channelActive(ChannelHandlerContext& ctx);

    virtual void channelInactive(ChannelHandlerContext& ctx);

    virtual void messageUpdated(ChannelHandlerContext& ctx);

    virtual void flush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future);

    virtual ChannelHandlerPtr clone();

    virtual std::string toString() const;

    void setIdleEventCallback(const IdleEventCallback& idleEventCallback) {
        if (idleEventCallback) {
            this->idleEventCallback = idleEventCallback;
        }
    }

private:
    void initialize(ChannelHandlerContext& ctx);
    void destroy();

private:
    void handleWriteCompleted(const ChannelFuturePtr& future);

    void handleIdle(ChannelHandlerContext& ctx, const IdleStateEvent& state);

    void handleReaderIdleTimeout(ChannelHandlerContext& ctx);
    void handleWriterIdleTimeout(ChannelHandlerContext& ctx);
    void handleAllIdleTimeout(ChannelHandlerContext& ctx);

private:
    int64_t readerIdleTimeMillis;
    TimeoutPtr readerIdleTimeout;
    Time lastReadTime;
    int readerIdleCount;

    TimeoutCallback readerIdleTimeCallback;

    int64_t writerIdleTimeMillis;
    TimeoutPtr writerIdleTimeout;
    Time lastWriteTime;
    int writerIdleCount;

    TimeoutCallback writerIdleTimeCallback;

    int64_t allIdleTimeMillis;
    TimeoutPtr allIdleTimeout;
    int allIdleCount;

    TimeoutCallback allIdleTimeCallback;

    int state; // 0 - none, 1 - initialized, 2 - destroyed

    IdleEventCallback idleEventCallback;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEHANDLER_H)

// Local Variables:
// mode: c++
// End:
