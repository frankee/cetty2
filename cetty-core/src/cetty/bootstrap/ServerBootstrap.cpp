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

#include <cetty/bootstrap/ServerBootstrap.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/EventLoop.h>
#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelInitializer.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/IpAddress.h>

#include <cetty/util/Exception.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/LengthFieldPrepender.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/ProtobufUtil.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::util;


using namespace cetty::handler::codec;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;

bool initializeChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<LengthFieldBasedFrameDecoder>("frameDecoder",
        LengthFieldBasedFrameDecoder::Ptr(
        new LengthFieldBasedFrameDecoder(
        16 * 1024 * 1024,
        0,
        4,
        0,
        4,
        4,
        ProtobufUtil::adler32Checksum)));

    pipeline.addLast<LengthFieldPrepender::Self>("frameEncoder",
        LengthFieldPrepender::Ptr(new LengthFieldPrepender(
        4,
        4,
        ProtobufUtil::adler32Checksum)));

    //pipeline->addLast("protoCodec", new ProtobufServiceCodec);
    pipeline.addLast<ProtobufServiceMessageDecoder::Self>(
        "protobufDecoder",
        ProtobufServiceMessageDecoder::Ptr(new ProtobufServiceMessageDecoder()));

    pipeline.addLast<ProtobufServiceMessageEncoder::Self>(
        "protobufEncoder",
        ProtobufServiceMessageEncoder::Ptr(new ProtobufServiceMessageEncoder()));

    pipeline.addLast<ProtobufServiceMessageHandler>(
        "messageHandler",
        ProtobufServiceMessageHandlerPtr(new ProtobufServiceMessageHandler()));

    return true;
}

class Acceptor : private boost::noncopyable {
public:
    typedef boost::shared_ptr<Acceptor> Ptr;
    typedef ChannelMessageContainer<ChannelPtr, MESSAGE_BLOCK> InboundContainer;
    typedef InboundContainer::MessageQueue InboundQueue;

    typedef ChannelMessageHandlerContext<
        Acceptor,
        ChannelPtr,
        VoidMessage,
        VoidMessage,
        VoidMessage,
        InboundContainer,
        VoidMessageContainer,
        VoidMessageContainer,
        VoidMessageContainer> Context;

public:
    Acceptor(ServerBootstrap& bootstrap)
        : bootstrap_(bootstrap),
          context_() {
    }

    ~Acceptor() {}

    void registerTo(Context& ctx) {
        context_ = &ctx;

        ctx.setChannelMessageUpdatedCallback(boost::bind(
            &Acceptor::messageUpdated,
            this,
            _1));
    }

    void messageUpdated(ChannelHandlerContext& ctx) {

        InboundQueue& inboundQueue = ctx.inboundMessageContainer<InboundContainer>()->getMessages();
        //InboundQueue& inboundQueue = context_->inboundContainer()->getMessages();

        while (!inboundQueue.empty()) {
            const ChannelPtr& child = inboundQueue.front();

            if (!child) {
                break;
            }

            child->setInitializer(boost::bind(
                initializeChannel,
                _1));

            //child->setInitializer(bootstrap_.childInitializer());

//             const ChannelOption::Options& childOptions =
//                 bootstrap_.getChildOptions();
// 
//             ChannelOption::Options::const_iterator itr = childOptions.begin();
//             for (; itr != childOptions.end(); ++itr) {
//                 if (!child->config().setOption(itr->first, itr->second)) {
//                     //logger.warn("Unknown channel option: " + e);
//                 }
//             }

            inboundQueue.pop_front();
        }
    }

private:
    ServerBootstrap& bootstrap_;
    Context* context_;
};

ChannelFuturePtr ServerBootstrap::bind(int port) {
    return bind(SocketAddress(IpAddress::IPv4, port));
}

ChannelFuturePtr ServerBootstrap::bind(const std::string& ip, int port) {
    return bind(SocketAddress(ip, port));
}

bool ServerBootstrap::initChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<Acceptor>("acceptor", Acceptor::Ptr(new Acceptor(*this)));

    return true;
}

ChannelFuturePtr ServerBootstrap::bind(const SocketAddress& localAddress) {
    // bossPipeline's life cycle will be managed by the server channel.

//     if (parentHandler) {
//         serverPipeline->addLast("userHandler", parentHandler->clone());
//     }

    ChannelPtr channel = newChannel();

    if (!channel) {
        LOG_ERROR << "Server channel factory failed to create a new channel.";
        return channel->newSucceededFuture();
    }

    channel->setInitializer(boost::bind(
        &ServerBootstrap::initChannel,
        this,
        _1));

    channel->open();

    if (channel->isOpen()) {
        LOG_INFO << "Created the AsioServerSocketChannel, firing the Channel Created Event.";
        //channel->pipeline().fireChannelOpen();

        serverChannels_.push_back(channel);
    }

    ChannelFuturePtr future = channel->newFuture();
    channel->bind(localAddress, future)->addListener(ChannelFutureListener::CLOSE_ON_FAILURE);

    return future;
}

ServerBootstrap& ServerBootstrap::setChildOption(const ChannelOption& option,
                                     const ChannelOption::Variant& value) {
    if (value.empty()) {
        childOptions_.erase(option);
        LOG_WARN << "setOption, the key ("
            << option.getName()
            << ") is empty value, remove from the options.";
    }
    else {
        LOG_INFO << "set Option, the key is " << option.getName();
        childOptions_.insert(std::make_pair(option, value));
    }

    return *this;
}

const ChannelOption::Options& ServerBootstrap::getChildOptions() const {
    return childOptions_;
}

}
}
