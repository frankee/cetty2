#if !defined(CETTY_CHANNEL_COMPLETECHANNELFUTURE_H)
#define CETTY_CHANNEL_COMPLETECHANNELFUTURE_H

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

#include <cetty/channel/ChannelFuture.h>

namespace cetty {
namespace channel {

/**
 * A skeletal {@link ChannelFuture} implementation which represents a
 * {@link ChannelFuture} which has been completed already.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class CompleteChannelFuture : public ChannelFuture {
public:
    virtual ~CompleteChannelFuture() {}

    virtual ChannelFuturePtr addListener(const CompletedCallback& listener, int priority = 0);
    virtual ChannelFuturePtr addProgressListener(const ProgressedCallback& listener);

    virtual ChannelFuturePtr removeListener(const CompletedCallback& listener);
    virtual ChannelFuturePtr removeProgressListener(const ProgressedCallback& listener);

    virtual ChannelFuturePtr await();
    virtual bool await(int64_t timeoutMillis);

    virtual ChannelFuturePtr awaitUninterruptibly();
    virtual bool awaitUninterruptibly(int64_t timeoutMillis);

    virtual ChannelPtr channel() const;
    //virtual ChannelPtr sharedChannel() const;

    virtual bool isDone() const;

    virtual bool setProgress(int amount, int current, int total);
    virtual bool setFailure(const Exception& cause);
    virtual bool setSuccess();

    virtual bool cancel();
    virtual bool isCancelled() const;

protected:
    /**
     * Creates a new instance.
     *
     * @param channel the {@link Channel} associated with this future
     */
    CompleteChannelFuture(const ChannelPtr& channel);

    CompleteChannelFuture(const ChannelWeakPtr& channel);

private:
    ChannelWeakPtr channel_;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_COMPLETECHANNELFUTURE_H)

// Local Variables:
// mode: c++
// End:
