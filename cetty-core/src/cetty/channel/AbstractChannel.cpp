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
#include <cetty/channel/AbstractChannel.h>

#include <boost/crc.hpp>
#include <boost/bind.hpp>

#include <cetty/channel/Channels.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelSink.h>
#include <cetty/channel/ChannelState.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/DownstreamMessageEvent.h>
#include <cetty/channel/DownstreamChannelStateEvent.h>
#include <cetty/channel/ChannelFutureListener.h>

#include <cetty/channel/SucceededChannelFuture.h>
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/FailedChannelFuture.h>

#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

AbstractChannel::ChannelMap AbstractChannel::allChannels;

class ChannelCloseFuture : public DefaultChannelFuture {
public:
    ChannelCloseFuture(const ChannelPtr& channel)
        : DefaultChannelFuture(channel, false) {
    }
    ~ChannelCloseFuture() {}

    bool setSuccess() {
        // User is not supposed to call this method - ignore silently.
        return false;
    }

    bool setFailure(const Exception& cause) {
        // User is not supposed to call this method - ignore silently.
        return false;
    }

    bool setClosed() {
        return DefaultChannelFuture::setSuccess();
    }
};

AbstractChannel::AbstractChannel(const ChannelPtr& parent, const ChannelFactoryPtr& factory, const ChannelPipelinePtr& pipeline, const ChannelSinkPtr& sink)
    : parent(parent), factory(factory), pipeline(pipeline), interestOps(OP_READ) {
    BOOST_ASSERT(factory && pipeline && sink && "input must not to be NULL!");
    init(pipeline, sink);
    id = allocateId(this);
}

AbstractChannel::AbstractChannel(int id, const ChannelPtr& parent, const ChannelFactoryPtr& factory, const ChannelPipelinePtr& pipeline, const ChannelSinkPtr& sink)
    : id(id), parent(parent), factory(factory), pipeline(pipeline), interestOps(OP_READ) {
    BOOST_ASSERT(factory && pipeline && sink && "input must not to be NULL!");
    init(pipeline, sink);
}

AbstractChannel::~AbstractChannel() {

}

ChannelFuturePtr AbstractChannel::write(const ChannelMessage& message) {
    return AbstractChannel::write(message, getRemoteAddress());
}

ChannelFuturePtr AbstractChannel::write(const ChannelMessage& message,
                                        const SocketAddress& remoteAddress) {
    ChannelFuturePtr future;

    //if (withFuture) {
    future = Channels::future(this);
    //}

    pipeline->sendDownstream(DownstreamMessageEvent(this, future, message, remoteAddress));

    return future;
}

ChannelFuturePtr AbstractChannel::unbind() {
    ChannelFuturePtr future = Channels::future(this);
    pipeline->sendDownstream(DownstreamChannelStateEvent(
                                 this, future, ChannelState::BOUND));

    return future;
}

ChannelFuturePtr AbstractChannel::close() {
    if (closeFuture->isDone()) {
        return closeFuture;
    }

    pipeline->sendDownstream(DownstreamChannelStateEvent(
                                 this, closeFuture, ChannelState::OPEN));

    return closeFuture;
}

ChannelFuturePtr AbstractChannel::disconnect() {
    ChannelFuturePtr future = Channels::future(this);
    pipeline->sendDownstream(DownstreamChannelStateEvent(
                                 this, future, ChannelState::CONNECTED));

    return future;
}

ChannelFuturePtr AbstractChannel::setInterestOps(int interestOps) {
    interestOps = Channels::validateAndFilterDownstreamInteresOps(interestOps);

    ChannelFuturePtr future = Channels::future(this);
    pipeline->sendDownstream(DownstreamChannelStateEvent(
                                 this, future, ChannelState::INTEREST_OPS, boost::any(interestOps)));
    return future;
}

void AbstractChannel::init(const ChannelPipelinePtr& pipeline,
                           const ChannelSinkPtr& sink) {
    pipeline->attach(this, sink);
    succeededFuture = new SucceededChannelFuture(this);
    closeFuture = new ChannelCloseFuture(this);

    closeFuture->addListener(
        boost::bind(&AbstractChannel::idDeallocatorCallback, this, _1));
}

int AbstractChannel::getId() const {
    return id;
}

const ChannelPtr& AbstractChannel::getParent() const {
    return parent;
}

const ChannelFactoryPtr& AbstractChannel::getFactory() const {
    return factory;
}

const ChannelPipelinePtr& AbstractChannel::getPipeline() const {
    return pipeline;
}

bool AbstractChannel::isOpen() const {
    return !closeFuture->isDone();
}

cetty::channel::ChannelFuturePtr AbstractChannel::bind(const SocketAddress& localAddress) {
    return Channels::bind(this, localAddress);
}

ChannelFuturePtr& AbstractChannel::getCloseFuture() {
    return this->closeFuture;
}

ChannelFuturePtr& AbstractChannel::getSucceededFuture() {
    return this->succeededFuture;
}

cetty::channel::ChannelFuturePtr AbstractChannel::connect(const SocketAddress& remoteAddress) {
    return Channels::connect(this, remoteAddress);
}

int AbstractChannel::getInterestOps() const {
    return this->interestOps;
}

bool AbstractChannel::isReadable() const {
    return (getInterestOps() & OP_READ) != 0;
}

bool AbstractChannel::isWritable() const {
    return (getInterestOps() & OP_WRITE) == 0;
}

cetty::channel::ChannelFuturePtr AbstractChannel::setReadable(bool readable) {
    if (readable) {
        return setInterestOps(getInterestOps() | OP_READ);
    }
    else {
        return setInterestOps(getInterestOps() & (~OP_READ));
    }
}

bool AbstractChannel::setClosed() {
    return boost::static_pointer_cast<ChannelCloseFuture>(closeFuture)->setClosed();
}

cetty::channel::ChannelFuturePtr AbstractChannel::getUnsupportedOperationFuture() {
    return ChannelFuturePtr(
               new FailedChannelFuture(this, UnsupportedOperationException()));
}

void AbstractChannel::setInterestOpsNow(int interestOps) {
    this->interestOps = interestOps;
}

void AbstractChannel::idDeallocatorCallback(ChannelFuture& future) {
    AbstractChannel::ChannelMap::iterator itr
        = AbstractChannel::allChannels.find(future.getChannel()->getId());

    if (itr != AbstractChannel::allChannels.end()) {
        AbstractChannel::allChannels.erase(itr);
    }
}

int AbstractChannel::allocateId(const ChannelPtr& channel) {
    int id = channel->hashCode();

    for (;;) {
        // Loop until a unique ID is acquired.
        // It should be found in one loop practically.
        if (allChannels.insert(std::make_pair<int, ChannelPtr>(id, channel)).second) {
            // Successfully acquired.
            return id;
        }
        else {
            // Taken by other channel at almost the same moment.
            id += 1;
        }
    }
}

int AbstractChannel::hashCode() const {
    boost::crc_32_type crc32;
    crc32.process_bytes((void const*)this, sizeof(this));
    return crc32.checksum();
}

std::string AbstractChannel::toString() const {
    char buf[512] = {0};

    bool connected = isConnected();

    if (connected && !strVal.empty()) {
        return strVal;
    }

    const SocketAddress& localAddress = getLocalAddress();
    const SocketAddress& remoteAddress = getRemoteAddress();

    if (remoteAddress.validated()) {
        if (NULL == getParent()) { // server channel or client channel
            sprintf(buf, "[id: 0x%08x, %s => %s]", getId(),
                    localAddress.toString().c_str(),
                    remoteAddress.toString().c_str());
        }
        else { // connection channel
            sprintf(buf, "[id: 0x%08x, %s => %s]", getId(),
                    remoteAddress.toString().c_str(),
                    localAddress.toString().c_str());
        }
    }
    else if (localAddress.validated()) {
        sprintf(buf, "[id: 0x%08x, %s]", getId(),
                localAddress.toString().c_str());
    }
    else {
        sprintf(buf, "[id: 0x%08x]", getId());
    }

    if (connected) {
        strVal = (const char*)buf;
    }
    else {
        strVal.empty();
    }

    return buf;
}

int AbstractChannel::compareTo(const ChannelPtr& c) const {
    if (c) {
        return (getId() - c->getId());
    }

    return 1;
}

}
}