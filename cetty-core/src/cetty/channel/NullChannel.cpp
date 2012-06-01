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
#include <cetty/channel/DefaultChannelPipeline.h>
#include <cetty/channel/FailedChannelFuture.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

class NullChannelFactory : public ChannelFactory {
public:
    NullChannelFactory() {}
    virtual ~NullChannelFactory() {}

    virtual ChannelPtr newChannel(const ChannelPipelinePtr& pipeline) {
        return NullChannel::getInstance();
    }

    virtual void releaseExternalResources() {
    }
};

static const Exception exception("NullChannel");
static DefaultChannelConfig channelConfig;
static ChannelPipelinePtr channelPipeline = new DefaultChannelPipeline;
static ChannelFactoryPtr nullChannelFactory = new NullChannelFactory;

ChannelPtr NullChannel::nullChannel = new NullChannel;
cetty::channel::ChannelFuturePtr NullChannel::failedFuture;

NullChannel::NullChannel() {
    if (!failedFuture) {
        failedFuture = new FailedChannelFuture(this, exception);
    }
}

int NullChannel::getId() const {
    return -1;
}

int NullChannel::hashCode() const {
    return 0;
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

ChannelFuturePtr NullChannel::write(const ChannelMessage& message) {
    return failedFuture;
}

ChannelFuturePtr NullChannel::write(const ChannelMessage& message,
                                    const SocketAddress& remoteAddress) {
    return failedFuture;
}

void NullChannel::write(const ChannelMessage& message,
                        ChannelFuturePtr* future) {

}

void NullChannel::write(const ChannelMessage& message,
                         const SocketAddress& remoteAddress,
                         ChannelFuturePtr* future) {

}

ChannelFuturePtr NullChannel::bind(const SocketAddress& localAddress) {
    return failedFuture;
}

ChannelFuturePtr NullChannel::connect(const SocketAddress& remoteAddress) {
    return failedFuture;
}

ChannelFuturePtr NullChannel::disconnect() {
    return failedFuture;
}

ChannelFuturePtr NullChannel::unbind() {
    return failedFuture;
}

ChannelFuturePtr NullChannel::close() {
    return failedFuture;
}

ChannelFuturePtr& NullChannel::getCloseFuture() {
    return failedFuture;
}

ChannelFuturePtr& NullChannel::getSucceededFuture() {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::setInterestOps(int interestOps) {
    return failedFuture;
}

cetty::channel::ChannelFuturePtr NullChannel::setReadable(bool readable) {
    return failedFuture;
}

const ChannelPtr& NullChannel::getParent() const {
    return fatherChannel;
}

std::string NullChannel::toString() const {
    return "NullChannle";
}

const ChannelPtr& NullChannel::getInstance() {
    return nullChannel;
}

bool NullChannel::isOpen() const {
    return false;
}

bool NullChannel::isBound() const {
    return false;
}

bool NullChannel::isConnected() const {
    return false;
}

int NullChannel::getInterestOps() const {
    return OP_NONE;
}

bool NullChannel::isReadable() const {
    return false;
}

bool NullChannel::isWritable() const {
    return false;
}

int NullChannel::compareTo(const ChannelPtr& c) const {
    if (c) {
        return getId() - c->getId();
    }

    return 1;
}

}
}