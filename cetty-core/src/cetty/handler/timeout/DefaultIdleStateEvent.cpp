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

#include <cetty/handler/timeout/DefaultIdleStateEvent.h>

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <cetty/channel/Channel.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;

std::string DefaultIdleStateEvent::toString() const {
    std::string str(channel->toString());

    str += " ";
    str += state.toString();
    str += " since ";
    str += boost::posix_time::to_simple_string(lastActivityTime);
    return str;
}

const ChannelFuturePtr& DefaultIdleStateEvent::getFuture() const {
    return channel->getSucceededFuture();
}

}
}
}
