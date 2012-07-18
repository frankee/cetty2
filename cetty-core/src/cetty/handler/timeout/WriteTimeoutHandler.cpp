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

#include <cetty/handler/timeout/WriteTimeoutHandler.h>

#include <boost/thread/thread_time.hpp>
#include <boost/bind.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/util/TimeUnit.h>
#include <cetty/util/Timeout.h>
#include <cetty/util/TimerFactory.h>

namespace cetty {
namespace handler {
namespace timeout {

const WriteTimeoutException WriteTimeoutHandler::EXCEPTION("WriteTimeout");

WriteTimeoutHandler::WriteTimeoutHandler(int timeoutSeconds) {
    if (timeoutSeconds <= 0) {
        timeoutMillis = 0;
    }
    else {
        timeoutMillis = TimeUnit::SECONDS.toMillis(timeoutSeconds);
    }
}

WriteTimeoutHandler::WriteTimeoutHandler(boost::int64_t timeout,
        const TimeUnit& unit) {
    if (timeout <= 0) {
        timeoutMillis = 0;
    }
    else {
        timeoutMillis = unit.toMillis(timeout);

        if (0 == timeoutMillis) { timeoutMillis = 1; }
    }
}

void WriteTimeoutHandler::releaseExternalResources() {
    if (timer) {
        timer->stop();
    }
}

void WriteTimeoutHandler::writeRequested(ChannelHandlerContext& ctx,
        const MessageEvent& e) {
    boost::int64_t timeoutMillis = getTimeoutMillis(e);

    if (timeoutMillis > 0) {
        if (!timer) {
            timer = TimerFactory::getFactory().getTimer(e.getChannel());
        }

        // Set timeout only when getTimeoutMillis() returns a positive value.
        const ChannelFuturePtr& future = e.getFuture();

        if (future) {
            const TimeoutPtr& timeout =
                timer->newTimeout(boost::bind(&WriteTimeoutHandler::handleWriteTimeout,
                                              this,
                                              _1,
                                              boost::ref(ctx),
                                              boost::cref(future)),
                                  timeoutMillis);

            future->addListener(boost::bind(&WriteTimeoutHandler::cancelTimeout,
                                            this,
                                            _1,
                                            timeout));
        }
    }

    ctx.sendDownstream(e);
}

void WriteTimeoutHandler::writeTimedOut(ChannelHandlerContext& ctx) {
    Channels::fireExceptionCaught(ctx, EXCEPTION);
}

void WriteTimeoutHandler::handleWriteTimeout(Timeout& timeout,
        ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    if (timeout.isCancelled()) {
        return;
    }

    if (!ctx.getChannel()->isOpen()) {
        return;
    }

    // Mark the future as failure
    if (future->setFailure(EXCEPTION)) {
        // If succeeded to mark as failure, notify the pipeline, too.
        try {
            writeTimedOut(ctx);
        }
        catch (const Exception& t) {
            // TODO
            // here may have problems:
            // if the timer is in the different thread (not the default AsioDeadlineTimer)
            // with this pipeline, and user override the
            // {@link writeTimedOut(ChannelHandlerContext&) writeTimedOut},
            // and also unfortunately, the writeTimedOut throw an exception,
            // then the pipeline will concurrently work in different threads.
            //
            // So at present, make sure handleWriteTimeout will be invoked in the
            // same thread with the channel's pipeline.
            Channels::fireExceptionCaught(ctx, t);
        }
        catch (const std::exception& t) {
            Channels::fireExceptionCaught(ctx, Exception(t.what()));
        }
        catch (...) {
            Channels::fireExceptionCaught(ctx, Exception("Unknow exception"));
        }
    }
}

void WriteTimeoutHandler::cancelTimeout(ChannelFuture& future,
                                        TimeoutPtr timeout) {
    timeout->cancel();
}

}
}
}
