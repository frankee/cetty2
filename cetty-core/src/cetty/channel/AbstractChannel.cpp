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

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/ChannelSinkHandler.h>
#include <cetty/channel/FailedChannelFuture.h>
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/SucceededChannelFuture.h>

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
        if (channel) {
            try {
                channel->doPreClose();
            }
            catch(const Exception& e) {
                LOG_WARN << "doPreClose() raised an exception: " << e.getDisplayText();
            }
        }

        return DefaultChannelFuture::setSuccess();
    }

private:
    boost::intrusive_ptr<AbstractChannel> channel;
};

AbstractChannel::AbstractChannel(const EventLoopPtr& eventLoop,
                                 const ChannelPtr& parent,
                                 const ChannelFactoryPtr& factory,
                                 const ChannelPipelinePtr& pipeline)
    : id(),
      eventLoop(eventLoop),
      parent(parent),
      factory(factory),
      pipeline(pipeline) {
    BOOST_ASSERT(factory && pipeline && "input must not to be NULL!");

    succeededFuture = new SucceededChannelFuture(shared_from_this());
    closeFuture = new ChannelCloseFuture(shared_from_this());

    if (!id) {
        id = allocateId(this);
    }

    closeFuture->addListener(
        boost::bind(&AbstractChannel::idDeallocatorCallback, this, _1));
}

AbstractChannel::AbstractChannel(int id,
                                 const EventLoopPtr& eventLoop,
                                 const ChannelPtr& parent,
                                 const ChannelFactoryPtr& factory,
                                 const ChannelPipelinePtr& pipeline)
    : id(id),
      eventLoop(eventLoop),
      parent(parent),
      factory(factory),
      pipeline(pipeline) {
    BOOST_ASSERT(factory && pipeline && "input must not to be NULL!");

    succeededFuture = new SucceededChannelFuture(shared_from_this());
    closeFuture = new ChannelCloseFuture(shared_from_this());

    if (!id) {
        id = allocateId(this);
    }

    closeFuture->addListener(
        boost::bind(&AbstractChannel::idDeallocatorCallback, this, _1));
}

AbstractChannel::~AbstractChannel() {
}

void AbstractChannel::setPipeline(const ChannelPipelinePtr& pipeline) {
    pipeline->setSinkHandler(
        new ChannelSinkHandler(
            boost::static_pointer_cast<AbstractChannel>(shared_from_this())));

    pipeline->attach(shared_from_this());
}

int AbstractChannel::getId() const {
    return id;
}

const EventLoopPtr& AbstractChannel::getEventLoop() const {
    return eventLoop;
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

ChannelFuturePtr& AbstractChannel::getCloseFuture() {
    return this->closeFuture;
}

bool AbstractChannel::setClosed() {
    return boost::static_pointer_cast<ChannelCloseFuture>(closeFuture)->setClosed();
}

ChannelFuturePtr AbstractChannel::getUnsupportedOperationFuture() {
    return new FailedChannelFuture(shared_from_this(),
                                   UnsupportedOperationException());
}

void AbstractChannel::idDeallocatorCallback(ChannelFuture& future) {
    AbstractChannel::ChannelMap::iterator itr
        = AbstractChannel::allChannels.find(future.getChannel()->getId());

    if (itr != AbstractChannel::allChannels.end()) {
        AbstractChannel::allChannels.erase(itr);
    }
}

int AbstractChannel::allocateId(const ChannelPtr& channel) {
    boost::crc_32_type crc32;
    crc32.process_bytes((void const*)this, sizeof(this));
    int id = crc32.checksum();

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

std::string AbstractChannel::toString() const {
    char buf[512] = {0};

    bool active = isActive();

    if (active && !strVal.empty()) {
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

    if (active) {
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

ChannelFuturePtr AbstractChannel::bind(const SocketAddress& localAddress) {
    return pipeline->bind(localAddress);
}

const ChannelFuturePtr& AbstractChannel::bind(const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    return pipeline->bind(localAddress, future);
}

ChannelFuturePtr AbstractChannel::connect(const SocketAddress& remoteAddress) {
    return pipeline->connect(remoteAddress);
}

ChannelFuturePtr AbstractChannel::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress) {
    return pipeline->connect(remoteAddress, localAddress);
}

const ChannelFuturePtr& AbstractChannel::connect(const SocketAddress& remoteAddress,
        const ChannelFuturePtr& future) {
    return pipeline->connect(remoteAddress, future);
}

const ChannelFuturePtr& AbstractChannel::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    return pipeline->connect(remoteAddress, localAddress, future);
}

ChannelFuturePtr AbstractChannel::disconnect() {
    return pipeline->disconnect();
}

const ChannelFuturePtr& AbstractChannel::disconnect(const ChannelFuturePtr& future) {
    return pipeline->disconnect(future);
}

ChannelFuturePtr AbstractChannel::close() {
    return pipeline->close();
}

const ChannelFuturePtr& AbstractChannel::close(const ChannelFuturePtr& future) {
    return pipeline->close(future);
}

ChannelFuturePtr AbstractChannel::flush() {
    return pipeline->flush();
}

const ChannelFuturePtr& AbstractChannel::flush(const ChannelFuturePtr& future) {
    return pipeline->flush(future);
}

ChannelFuturePtr AbstractChannel::newFuture() {
    return new DefaultChannelFuture(shared_from_this(), false);
}

ChannelFuturePtr AbstractChannel::newFailedFuture(const Exception& e) {
    return new FailedChannelFuture(shared_from_this(), e);
}

ChannelFuturePtr AbstractChannel::newSucceededFuture() {
    return succeededFuture;
}

}
}
