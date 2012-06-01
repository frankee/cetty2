#if !defined(CETTY_CHANNEL_CHANNELFUTURELISTENER_H)
#define CETTY_CHANNEL_CHANNELFUTURELISTENER_H

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

class ChannelFutureListener {
public:
    /**
     * A {@link ChannelFutureListener} that closes the {@link Channel} which is
     * associated with the specified {@link ChannelFuture}.
     */
    static ChannelFuture::CompletedCallback CLOSE;

    /**
     * A {@link ChannelFutureListener} that closes the {@link Channel} when the
     * operation ended up with a failure or cancellation rather than a success.
     */
    static ChannelFuture::CompletedCallback CLOSE_ON_FAILURE;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_CHANNELFUTURELISTENER_H)

// Local Variables:
// mode: c++
// End:

