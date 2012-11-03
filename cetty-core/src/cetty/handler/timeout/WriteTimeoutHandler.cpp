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

#include <boost/bind.hpp>
#include <boost/thread/thread_time.hpp>

#include <cetty/channel/Timeout.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace handler {
namespace timeout {

const WriteTimeoutException WriteTimeoutHandler::EXCEPTION("WriteTimeout");

WriteTimeoutHandler::WriteTimeoutHandler(int timeoutSeconds)
    : timeoutMillis(timeoutSeconds * 1000) {
    if (timeoutSeconds <= 0) {
        timeoutMillis = 0;
    }
}

WriteTimeoutHandler::WriteTimeoutHandler(const Duration& timeout)
    : timeoutMillis(timeout.total_milliseconds()) {
    if (timeoutMillis <= 0) {
        timeoutMillis = 0;
    }
}

void WriteTimeoutHandler::flush(ChannelHandlerContext& ctx,
    const ChannelFuturePtr& future) {
    if (timeoutMillis > 0) {
        // Schedule a timeout.
        ctx.getEventLoop()->runAfter(timeoutMillis, boost::bind(
            &WriteTimeoutHandler::handleWriteTimeout,
            this,
            boost::ref(ctx),
            boost::cref(future)));

        // Cancel the scheduled timeout if the flush future is complete.
        future->addListener(boost::bind(
            &WriteTimeoutHandler::cancelTimeout,
            this,
            _1));
    }

    AbstractChannelOutboundHandler::flush(ctx, future);
}

ChannelHandlerPtr WriteTimeoutHandler::clone() {
    return new WriteTimeoutHandler(
        boost::posix_time::milliseconds(timeoutMillis));
}

std::string WriteTimeoutHandler::toString() const {
    return "WriterTimeoutHandler";
}

void WriteTimeoutHandler::handleWriteTimeout(ChannelHandlerContext& ctx,
    const ChannelFuturePtr& future) {

    // Mark the future as failure
    if (future->setFailure(EXCEPTION)) {
        // If succeeded to mark as failure, notify the pipeline, too.
        try {
            writeTimedOut(ctx);
        }
        catch (const std::exception& t) {
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
            ctx.fireExceptionCaught(ChannelException(t.what()));
        }
        catch (...) {
            ctx.fireExceptionCaught(ChannelException("Unknown exception"));
        }
    }
}

void WriteTimeoutHandler::cancelTimeout(ChannelFuture& future) {
    if (timeout) {
        timeout->cancel();
    }
}

void WriteTimeoutHandler::writeTimedOut(ChannelHandlerContext& ctx) {
    if (!closed) {
        ctx.fireExceptionCaught(EXCEPTION);
        ctx.close();
        closed = true;
    }
}

}
}
}
