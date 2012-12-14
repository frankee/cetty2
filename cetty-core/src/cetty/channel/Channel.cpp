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

#include <boost/bind.hpp>

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/VoidChannelFuture.h>
#include <cetty/channel/FailedChannelFuture.h>
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/SucceededChannelFuture.h>

#include <cetty/util/Adler32.h>
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
        ChannelPtr ch = channel();
        if (!ch) {
            try {
                ch->doPreClose();
            }
            catch (const Exception& e) {
                LOG_WARN << "doPreClose() raised an exception: " << e.getDisplayText();
            }
        }

        return DefaultChannelFuture::setSuccess();
    }
};

Channel::Channel(const ChannelPtr& parent,
                 const EventLoopPtr& eventLoop)
    : id_(),
      parent_(parent),
      eventLoop_(eventLoop),
      pipeline_() {
    allocateId();
}

Channel::Channel(int id,
                 const ChannelPtr& parent,
                 const EventLoopPtr& eventLoop)
    : id_(id),
      parent_(parent),
      eventLoop_(eventLoop),
      pipeline_() {
    allocateId();
}

Channel::~Channel() {
    LOG_DEBUG << "Channel dctr";

    if (pipeline_) {
        delete pipeline_;
    }
}

void Channel::initialize() {
    ChannelPtr self = shared_from_this();

    succeededFuture_ = new SucceededChannelFuture(self);
    closeFuture_ = new ChannelCloseFuture(self);
    pipeline_ = new ChannelPipeline(self);

    if (initializer_) {
        initializer_(self);
    }

    doInitialize();

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
}

ChannelFuturePtr Channel::newFuture() {
    return new DefaultChannelFuture(shared_from_this(), false);
}

ChannelFuturePtr Channel::newFailedFuture(const Exception& e) {
    return new FailedChannelFuture(shared_from_this(), e);
}

ChannelFuturePtr Channel::newVoidFuture() {
    return new VoidChannelFuture(shared_from_this());
}

bool Channel::setClosed() {
    return boost::static_pointer_cast<ChannelCloseFuture>(closeFuture_)->setClosed();
}

void Channel::allocateId() {
    if (!id_) {
        id_ = Adler32::adler32(reinterpret_cast<char const*>(this),
                               sizeof(this));
    }
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

void Channel::doBind(ChannelHandlerContext& ctx,
                     const SocketAddress& localAddress,
                     const ChannelFuturePtr& future) {
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
        }

        Channel::setClosed();
    }
    else {
        // Closed already.
        future->setSuccess();
    }
}

void Channel::closeIfClosed() {
    if (isOpen()) {
        return;
    }

    //close(voidFuture());
}

}
}
