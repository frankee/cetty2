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

#include <cetty/channel/NullChannel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/DefaultChannelConfig.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/FailedChannelFuture.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

class NullChannelFactory : public ChannelFactory {
public:
    NullChannelFactory() {}
    virtual ~NullChannelFactory() {}

    virtual ChannelPtr newChannel(const ChannelPipelinePtr& pipeline) {
        return NullChannel::instance();
    }

    virtual void shutdown() {
    }
};

static const Exception exception("NullChannel");

static DefaultChannelConfig channelConfig;
static ChannelPipelinePtr channelPipeline = new ChannelPipeline;
static ChannelFactoryPtr nullChannelFactory = new NullChannelFactory;

ChannelFuturePtr NullChannel::failedFuture;
ChannelPtr NullChannel::nullChannel = new NullChannel;

NullChannel::NullChannel() {
    if (!failedFuture) {
        failedFuture = new FailedChannelFuture(this, exception);
    }
}

int NullChannel::getId() const {
    return -1;
}

const ChannelFactoryPtr& NullChannel::getFactory() const {
    return nullChannelFactory;
}

ChannelConfig& NullChannel::getConfig() {
    return channelConfig;
}

const ChannelConfig& NullChannel::getConfig() const {
    return channelConfig;
}

const ChannelPipelinePtr& NullChannel::getPipeline() const {
    return channelPipeline;
}

const SocketAddress& NullChannel::getLocalAddress() const {
    return SocketAddress::NULL_ADDRESS;
}

const SocketAddress& NullChannel::getRemoteAddress() const {
    return SocketAddress::NULL_ADDRESS;
}

const ChannelPtr& NullChannel::getParent() const {
    return fatherChannel;
}

std::string NullChannel::toString() const {
    return "NullChannle";
}

const ChannelPtr& NullChannel::instance() {
    return nullChannel;
}

bool NullChannel::isOpen() const {
    return false;
}

bool NullChannel::isActive() const {
    return false;
}

int NullChannel::compareTo(const ChannelPtr& c) const {
    if (c) {
        return getId() - c->getId();
    }

    return 1;
}

const EventLoopPtr& NullChannel::getEventLoop() const {
    return eventLoop;
}

ChannelFuturePtr NullChannel::bind(const SocketAddress& localAddress) {
    return failedFuture;
}

const ChannelFuturePtr& NullChannel::bind(const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    return future;
}

ChannelFuturePtr NullChannel::connect(const SocketAddress& remoteAddress) {
    return failedFuture;
}

ChannelFuturePtr NullChannel::connect(const SocketAddress& remoteAddress,
                                      const SocketAddress& localAddress) {
    return failedFuture;
}

const ChannelFuturePtr& NullChannel::connect(const SocketAddress& remoteAddress,
        const ChannelFuturePtr& future) {
    return future;
}

const ChannelFuturePtr& NullChannel::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    return future;
}

ChannelFuturePtr NullChannel::disconnect() {
    return failedFuture;
}

const ChannelFuturePtr& NullChannel::disconnect(const ChannelFuturePtr& future) {
    return future;
}

ChannelFuturePtr NullChannel::close() {
    return failedFuture;
}

const ChannelFuturePtr& NullChannel::close(const ChannelFuturePtr& future) {
    return future;
}

ChannelFuturePtr NullChannel::flush() {
    return failedFuture;
}

const ChannelFuturePtr& NullChannel::flush(const ChannelFuturePtr& future) {
    return future;
}

ChannelFuturePtr NullChannel::newFuture() {
    return failedFuture;
}

ChannelFuturePtr NullChannel::newFailedFuture(const Exception& e) {
    return failedFuture;
}

ChannelFuturePtr NullChannel::newSucceededFuture() {
    return failedFuture;
}

const ChannelFuturePtr& NullChannel::getCloseFuture() {
    return failedFuture;
}

}
}
