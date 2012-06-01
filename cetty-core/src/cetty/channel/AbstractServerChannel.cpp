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

#include <cetty/channel/AbstractServerChannel.h>

namespace cetty { namespace channel { 


AbstractServerChannel::AbstractServerChannel(const ChannelFactoryPtr& factory, const ChannelPipelinePtr& pipeline, const ChannelSinkPtr& sink) : AbstractChannel(ChannelPtr(), factory, pipeline, sink) {

}

AbstractServerChannel::~AbstractServerChannel() {

}

cetty::channel::ChannelFuturePtr AbstractServerChannel::connect(const SocketAddress& remoteAddress) {
    return AbstractChannel::getUnsupportedOperationFuture();
}

cetty::channel::ChannelFuturePtr AbstractServerChannel::disconnect() {
    return AbstractChannel::getUnsupportedOperationFuture();
}

int AbstractServerChannel::getInterestOps() {
    return OP_NONE;
}

cetty::channel::ChannelFuturePtr AbstractServerChannel::setInterestOps(int interestOps) {
    return AbstractChannel::getUnsupportedOperationFuture();
}

cetty::channel::ChannelFuturePtr AbstractServerChannel::write(const ChannelMessage& message) {
    return AbstractChannel::getUnsupportedOperationFuture();
}

cetty::channel::ChannelFuturePtr AbstractServerChannel::write(const ChannelMessage& message, const SocketAddress& remoteAddress) {
    return AbstractChannel::getUnsupportedOperationFuture();
}

bool AbstractServerChannel::isConnected() const {
    return false;
}

void AbstractServerChannel::setInterestOpsNow(int interestOps) {
    // Ignore.
}

}}