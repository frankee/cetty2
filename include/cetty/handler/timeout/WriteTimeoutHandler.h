#if !defined(CETTY_HANDLER_TIMEOUT_WRITETIMEOUTHANDLER_H)
#define CETTY_HANDLER_TIMEOUT_WRITETIMEOUTHANDLER_H

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

#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/channel/TimeoutPtr.h>
#include <cetty/channel/ChannelStateHandler.h>
#include <cetty/channel/ChannelFutureListener.h>

#include <cetty/handler/timeout/WriteTimeoutException.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;

/**
 * Raises a {@link WriteTimeoutException} when no data was written within a
 * certain period of time.
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
 *         // An example configuration that implements 30-second write timeout:
 *         return {@link Channels}.pipeline(
 *             <b>new {@link WriteTimeoutHandler}(timer, 30), // timer must be shared.</b>
 *             new MyHandler());
 *     }
 * }
 *
 * {@link ServerBootstrap} bootstrap = ...;
 * {@link Timer} timer = new {@link HashedWheelTimer}();
 * ...
 * bootstrap.setPipelineFactory(new MyPipelineFactory(timer));
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
 * @see IdleStateHandler
 *
 * @apiviz.landmark
 * @apiviz.uses org.jboss.netty.util.HashedWheelTimer
 * @apiviz.has org.jboss.netty.handler.timeout.TimeoutException oneway - - raises
 */

class WriteTimeoutHandler : private boost::noncopyable {
public:
    typedef boost::posix_time::ptime Time;
    typedef boost::posix_time::time_duration Duration;

    typedef boost::function<void (ChannelHandlerContext&)> WriteTimeoutCallback;

    typedef ChannelStateHandler<WriteTimeoutHandler>::Context Context;

public:
    /**
     * Creates a new instance.
     *
     * @param timer
     *        the {@link Timer} that is used to trigger the scheduled event.
     *        The recommended {@link Timer} implementation is {@link HashedWheelTimer}.
     * @param timeoutSeconds
     *        write timeout in seconds
     */
    WriteTimeoutHandler(int timeoutSeconds);

    /**
     * Creates a new instance.
     *
     * @param timer
     *        the {@link Timer} that is used to trigger the scheduled event.
     *        The recommended {@link Timer} implementation is {@link HashedWheelTimer}.
     * @param timeout
     *        write timeout
     * @param unit
     *        the {@link TimeUnit} of <tt>timeout</tt>
     */
    WriteTimeoutHandler(const Duration& timeout);

    void registerTo(Context& ctx) {
        ctx.setFlushFunctor(boost::bind(&WriteTimeoutHandler::flush,
                                        this,
                                        _1,
                                        _2));
    }

    void setWriteTimeoutCallback(const WriteTimeoutCallback& callback) {
        timeoutCallback_ = callback;
    }

private:
    void flush(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future);

    void handleWriteTimeout(ChannelHandlerContext& ctx,
                            const ChannelFuturePtr& future);

    void onWriteTimedOut(ChannelHandlerContext& ctx);

    void cancelTimeout(ChannelFuture& future);

private:
    static const WriteTimeoutException EXCEPTION;

private:
    bool closed_;
    boost::int64_t timeoutMillis_;

    TimeoutPtr timeout_;

    WriteTimeoutCallback timeoutCallback_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_WRITETIMEOUTHANDLER_H)

// Local Variables:
// mode: c++
// End:
