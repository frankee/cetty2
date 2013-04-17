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
#include <cetty/channel/ChannelStateHandler.h>

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

class IdleStateHandler : private boost::noncopyable {
public:
    typedef boost::posix_time::ptime Time;
    typedef boost::posix_time::time_duration Duration;

    typedef boost::function2<void, ChannelHandlerContext&, const IdleStateEvent&> IdleEventCallback;

    typedef ChannelStateHandler<IdleStateHandler>::Context Context;

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
    int64_t readerIdleTimeInMillis() const {
        return readerIdleTimeMillis_;
    }

    /**
     * Return the writerIdleTime that was given when instance this class in milliseconds.
     *
     */
    int64_t writerIdleTimeInMillis() const {
        return writerIdleTimeMillis_;
    }

    /**
     * Return the allIdleTime that was given when instance this class in milliseconds.
     *
     */
    int64_t allIdleTimeInMillis() const {
        return allIdleTimeMillis_;
    }

    void setIdleEventCallback(const IdleEventCallback& idleEventCallback) {
        if (idleEventCallback) {
            this->idleEventCallback_ = idleEventCallback;
        }
    }

    void registerTo(Context& ctx) {
        ctx.setBeforeAddCallback(boost::bind(
                                     &IdleStateHandler::beforeAdd,
                                     this,
                                     _1));
        ctx.setBeforeRemoveCallback(boost::bind(
                                        &IdleStateHandler::beforeRemove,
                                        this,
                                        _1));
        ctx.setChannelActiveCallback(boost::bind(
                                         &IdleStateHandler::channelActive,
                                         this,
                                         _1));
        ctx.setChannelInactiveCallback(boost::bind(
                                           &IdleStateHandler::channelInactive,
                                           this,
                                           _1));
        ctx.setChannelMessageUpdatedCallback(boost::bind(
                &IdleStateHandler::messageUpdated,
                this,
                _1));
        ctx.setFlushFunctor(boost::bind(
                                &IdleStateHandler::flush,
                                this,
                                _1,
                                _2));
    }

private:
    void beforeAdd(ChannelHandlerContext& ctx);
    void beforeRemove(ChannelHandlerContext& ctx);

    void channelActive(ChannelHandlerContext& ctx);
    void channelInactive(ChannelHandlerContext& ctx);

    void messageUpdated(ChannelHandlerContext& ctx);
    void flush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future);

    void initialize(ChannelHandlerContext& ctx);
    void destroy();

private:
    void handleWriteCompleted(const ChannelFuturePtr& future);

    void handleIdle(ChannelHandlerContext& ctx, const IdleStateEvent& state);

    void handleReaderIdleTimeout(ChannelHandlerContext& ctx);
    void handleWriterIdleTimeout(ChannelHandlerContext& ctx);
    void handleAllIdleTimeout(ChannelHandlerContext& ctx);

private:
    typedef boost::function0<void> TimerCallback;

private:
    int64_t readerIdleTimeMillis_;
    TimeoutPtr readerIdleTimeout_;
    Time lastReadTime_;
    int readerIdleCount_;

    TimerCallback readerIdleTimerCallback_;

    int64_t writerIdleTimeMillis_;
    TimeoutPtr writerIdleTimeout_;
    Time lastWriteTime_;
    int writerIdleCount_;

    TimerCallback writerIdleTimerCallback_;

    int64_t allIdleTimeMillis_;
    TimeoutPtr allIdleTimeout_;
    int allIdleCount_;

    TimerCallback allIdleTimeCallback_;

    int state_; // 0 - none, 1 - initialized, 2 - destroyed

    IdleEventCallback idleEventCallback_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEHANDLER_H)

// Local Variables:
// mode: c++
// End:
