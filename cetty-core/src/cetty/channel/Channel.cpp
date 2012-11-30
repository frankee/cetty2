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

#include <boost/crc.hpp>
#include <boost/bind.hpp>

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/FailedChannelFuture.h>
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/SucceededChannelFuture.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace channel {

class ChannelCloseFuture : public DefaultChannelFuture {
public:
    ChannelCloseFuture(const ChannelPtr& channel)
        : DefaultChannelFuture(channel, false) {
    }
    ~ChannelCloseFuture() {}

    virtual bool setSuccess() {
        // User is not supposed to call this method - ignore silently.
        return false;
    }

    virtual bool setFailure(const Exception& cause) {
        // User is not supposed to call this method - ignore silently.
        return false;
    }

    bool setClosed() {
        if (channel) {
            try {
                channel->doPreClose();
            }
            catch (const Exception& e) {
                LOG_WARN << "doPreClose() raised an exception: " << e.getDisplayText();
            }
        }

        return DefaultChannelFuture::setSuccess();
    }

private:
    ChannelPtr channel;
};

Channel::Channel(const ChannelPtr& parent,
                 const EventLoopPtr& eventLoop)
    : id_(),
      parent_(parent),
      eventLoop_(eventLoop),
      pipeline_(),
      context_() {
}

Channel::Channel(int id,
                 const ChannelPtr& parent,
                 const EventLoopPtr& eventLoop)
    : id_(id),
      parent_(parent),
      eventLoop_(eventLoop),
      pipeline_(),
      context_() {
}

Channel::~Channel() {
    LOG_DEBUG << "Channel dectr";

    if (pipeline_) {
        delete pipeline_;
    }
}

void Channel::init() {
}

void Channel::open() {
    ChannelPtr self = shared_from_this();

    if (!id_) {
        id_ = allocateId();
    }

#if 0  // FIXME need concurrent hash map
    closeFuture->addListener(
        boost::bind(&Channel::idDeallocatorCallback, this, _1));
#endif

    succeededFuture_ = new SucceededChannelFuture(self);
    closeFuture_ = new ChannelCloseFuture(self);
    pipeline_ = new ChannelPipeline(self);

    if (initializer_) {
        initializer_(self);
    }

    //pipeline_->addFirst(new Context("bridge", self));
    pipeline_->addFirst<ChannelPtr>("bridge", self);
    pipeline_->fireChannelOpen();
}

ChannelFuturePtr Channel::close() {
    if (pipeline_) {
        return pipeline_->close();
    }
    else {
        LOG_INFO << "close the channel, but the pipeline has detached.";
        return closeFuture_;
    }
}

const ChannelFuturePtr& Channel::close(const ChannelFuturePtr& future) {
    return pipeline_->close(future);

    //if (pipeline_->attached()) {
    //    return pipeline_->close(future);
    //}
    //else {
    //    LOG_INFO << "close the channel, but the pipeline has detached.";
    //    return closeFuture_;
    //}
}

ChannelFuturePtr Channel::newFuture() {
    return new DefaultChannelFuture(shared_from_this(), false);
}

ChannelFuturePtr Channel::newFailedFuture(const Exception& e) {
    return new FailedChannelFuture(shared_from_this(), e);
}

ChannelFuturePtr Channel::newSucceededFuture() {
    return succeededFuture_;
}

const ChannelFuturePtr& Channel::closeFuture() {
    return this->closeFuture_;
}

bool Channel::setClosed() {
    return boost::static_pointer_cast<ChannelCloseFuture>(closeFuture_)->setClosed();
}

void Channel::idDeallocatorCallback(ChannelFuture& future) {
    //     std::map<int, ChannelPtr>::iterator itr
    //         = Channel::allChannels.find(future.getChannel()->id());
    //
    //     if (itr != Channel::allChannels.end()) {
    //         Channel::allChannels.erase(itr);
    //     }
}

int Channel::allocateId() {
    boost::crc_32_type crc32;
    crc32.process_bytes((void const*)this, sizeof(this));
    int id = crc32.checksum();

#if 0 //FIXME need concurrent hash map

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

#endif

    return id;
}

int Channel::compareTo(const ChannelPtr& c) const {
    if (c) {
        return (id() - c->id());
    }

    return 1;
}

std::string Channel::toString() const {
    bool active = isActive();

    if (!active) {
        strVal_.empty();
        return strVal_;
    }

    if (!strVal_.empty()) {
        return strVal_;
    }

    const SocketAddress& local = localAddress();
    const SocketAddress& remote = remoteAddress();

    if (remote.validated()) {
        if (!parent()) { // server channel or client channel
            StringUtil::printf(&strVal_, "[id: 0x%08x, %s => %s]", id(),
                               local.toString().c_str(),
                               remote.toString().c_str());
        }
        else { // connection channel
            StringUtil::printf(&strVal_, "[id: 0x%08x, %s => %s]", id(),
                               remote.toString().c_str(),
                               local.toString().c_str());
        }
    }
    else if (local.validated()) {
        StringUtil::printf(&strVal_, "[id: 0x%08x, %s]", id(),
                           local.toString().c_str());
    }
    else {
        StringUtil::printf(&strVal_, "[id: 0x%08x]", id());
    }

    return strVal_;
}

void Channel::registerTo(Context& context) {
    context_ = &context;

    context.setBindFunctor(boost::bind(&Channel::doBind,
                                       this,
                                       _1,
                                       _2,
                                       _3));

    context.setDisconnectFunctor(boost::bind(&Channel::doDisconnect,
                                 this,
                                 _1,
                                 _2));

    context.setCloseFunctor(boost::bind(&Channel::doClose,
                                        this,
                                        _1,
                                        _2));
}

void Channel::doBind(ChannelHandlerContext& ctx, const SocketAddress& localAddress, const ChannelFuturePtr& future) {
    if (!isOpen()) {
        return;
    }

    try {
        bool wasActive = isActive();
        doBind(localAddress);
        future->setSuccess();

        if (!wasActive && isActive()) {
            pipeline_->fireChannelActive();
        }
    }
    catch (const std::exception& e) {
        //future->setFailure(e);
        //channel.pipeline->fireExceptionCaught(t);
        closeIfClosed();
    }
}

void Channel::doDisconnect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    try {
        bool wasActive = isActive();
        doDisconnect();
        future->setSuccess();

        if (wasActive && !isActive()) {
            pipeline_->fireChannelInactive();
        }
    }
    catch (const std::exception& e) {
        //future.setFailure(t);
        closeIfClosed();
    }
}

void Channel::doClose(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    bool wasActive = isActive();

    if (setClosed()) {
        try {
            doClose();
            future->setSuccess();
        }
        catch (const std::exception& e) {
            //future->setFailure(t);
        }

        //if (closedChannelException != null) {
        //    closedChannelException = new ClosedChannelException();
        //}

        //notifyFlushFutures(closedChannelException);

        if (wasActive && !isActive()) {
            //LOG_INFO(logger, "closed the socket channel, finally firing channel closed event.");
            pipeline_->fireChannelInactive();

            closeFuture_.reset();
            succeededFuture_.reset();
        }
    }
    else {
        // Closed already.
        future->setSuccess();
    }
}

}
}
