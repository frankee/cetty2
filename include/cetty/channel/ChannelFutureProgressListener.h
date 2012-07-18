#if !defined(CETTY_CHANNEL_CHANNELFUTUREPROGRESSLISTENER_H)
#define CETTY_CHANNEL_CHANNELFUTUREPROGRESSLISTENER_H

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

#include <cetty/channel/ChannelFutureListener.h>

namespace cetty {
namespace channel {

/**
 * Listens to the progress of a time-consuming I/O operation such as a large
 * file transfer.  If this listener is added to a {@link ChannelFuture} of an
 * I/O operation that supports progress notification, the listener's
 * {@link #operationProgressed(const ChannelFuturePtr&, int, int, int)} method will be
 * called back by an I/O thread.  If the operation does not support progress
 * notification, {@link #operationProgressed(const ChannelFuturePtr&, int, int, int)}
 * will not be invoked.  Like a usual {@link ChannelFutureListener} that this
 * interface extends, {@link #operationComplete(const ChannelFuturePtr&)} will be called
 * when the future is marked as complete.
 *
 * <h3>Return the control to the caller quickly</h3>
 *
 * {@link #operationProgressed(const ChannelFuturePtr&, int, int, int)} and
 * {@link #operationComplete(const ChannelFuturePtr&)} is directly called by an I/O
 * thread.  Therefore, performing a time consuming task or a blocking operation
 * in the handler method can cause an unexpected pause during I/O.  If you need
 * to perform a blocking operation on I/O completion, try to execute the
 * operation in a different thread using a thread pool.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ChannelFutureProgressListener : public ChannelFutureListener {
public:
    virtual ~ChannelFutureProgressListener() {}

    /**
     * Invoked when the I/O operation associated with the {@link ChannelFuture}
     * has been progressed.
     *
     * @param future  the source {@link ChannelFuture} which called this
     *                callback
     */
    virtual void operationProgressed(const ChannelFuturePtr& future, int amount, int current, int total) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELFUTUREPROGRESSLISTENER_H)

// Local Variables:
// mode: c++
// End:
