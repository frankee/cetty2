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

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelEvent.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipelineException.h>
#include <cetty/channel/Channels.h>

#include <cetty/channel/AbstractChannelSink.h>

namespace cetty {
namespace channel {

AbstractChannelSink::AbstractChannelSink() {
}

void AbstractChannelSink::eventSunk(const ChannelPipeline& pipeline,
                                    const ChannelEvent& e) {
}

void AbstractChannelSink::exceptionCaught(const ChannelPipeline& pipeline,
        const ChannelEvent& e,
        const ChannelPipelineException& cause) {
    Channels::fireExceptionCaught(e.getChannel(), cause);
}

}
}
