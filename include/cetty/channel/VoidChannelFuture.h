#if !defined(CETTY_CHANNEL_VOIDCHANNELFUTURE_H)
#define CETTY_CHANNEL_VOIDCHANNELFUTURE_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/channel/ChannelFuture.h>

namespace cetty {
namespace channel {

class VoidChannelFuture : public ChannelFuture {
public:
    VoidChannelFuture(const ChannelPtr& channel)
        : channel_(channel) {}

    virtual ~VoidChannelFuture() {}

    virtual const ChannelWeakPtr& channel() const;
    virtual ChannelPtr sharedChannel() const;

    virtual bool isDone() const;
    virtual bool isCancelled() const;
    virtual bool isSuccess() const;

    virtual const Exception* failedCause() const;

    virtual bool cancel();
    virtual bool setSuccess();
    virtual bool setFailure(const Exception& cause);
    virtual bool setProgress(int amount, int current, int total);

    virtual ChannelFuturePtr addListener(const CompletedCallback& listenerr, int priority = 0);
    virtual ChannelFuturePtr addProgressListener(const ProgressedCallback& listener);

    virtual ChannelFuturePtr removeListener(const CompletedCallback& listener);
    virtual ChannelFuturePtr removeProgressListener(const ProgressedCallback& listener);

    virtual ChannelFuturePtr await();
    virtual ChannelFuturePtr awaitUninterruptibly();

    virtual bool await(int64_t timeoutMillis);
    virtual bool awaitUninterruptibly(int64_t timeoutMillis);

private:
    void fail();

private:
    ChannelWeakPtr channel_;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_VOIDCHANNELFUTURE_H)

// Local Variables:
// mode: c++
// End:
