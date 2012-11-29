#if !defined(CETTY_CHANNEL_FAILEDCHANNELFUTURE_H)
#define CETTY_CHANNEL_FAILEDCHANNELFUTURE_H

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

#include <cetty/channel/CompleteChannelFuture.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

/**
 * The {@link CompleteChannelFuture} which is failed already.  It is
 * recommended to use {@link Channels#failedFuture(Channel, Exception)}
 * instead of calling the constructor of this future.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class FailedChannelFuture : public CompleteChannelFuture {
public:
    /**
     * Creates a new instance.
     *
     * @param channel the {@link Channel} associated with this future
     * @param cause   the cause of failure
     */
    FailedChannelFuture(const ChannelPtr& channel, const Exception& cause);
    FailedChannelFuture(const ChannelWeakPtr& channel, const Exception& cause);

    virtual ~FailedChannelFuture();

    const Exception* failedCause() const;
    bool isSuccess() const;

private:
    cetty::util::Exception cause_;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_FAILEDCHANNELFUTURE_H)

// Local Variables:
// mode: c++
// End:
