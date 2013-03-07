#if !defined(CETTY_SERVICE_BUILDER_CLIENTBUILDER_H)
#define CETTY_SERVICE_BUILDER_CLIENTBUILDER_H

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

#include <map>
#include <boost/noncopyable.hpp>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/service/ClientService.h>
#include <cetty/service/ClientServiceDispatcher.h>

namespace cetty {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::channel::asio;
using namespace cetty::service;

class ClientBuilderConfig;

template<typename Request, typename Response = Request>
class ClientBuilder : private boost::noncopyable {
public:
    typedef ClientBuilder<Request, Response> Self;
    typedef ClientService<Request, Response> ServiceChannel;

public:
    ClientBuilder()
        : eventLoopPool_(new AsioServicePool(1)) {
    }
    ClientBuilder(int threadCnt)
        : eventLoopPool_(new AsioServicePool(threadCnt)) {
    }
    ClientBuilder(const EventLoopPtr& eventLoop)
        : eventLoop_(eventLoop) {
    }
    ClientBuilder(const EventLoopPoolPtr& eventLoopPool)
        : eventLoopPool_(eventLoopPool) {
    }

    virtual ~ClientBuilder() {}

    void setServiceInitializer(const Channel::Initializer& initializer) {
        serviceInitializer_ = initializer;
    }

    void setClientInitializer(const Channel::Initializer& initializer) {
        clientInitializer_ = initializer;
    }

    void addConnection(const std::string& host, int port, int limit = 1) {
        connections_.push_back(Connection(host, port, limit));
    }

    ChannelPtr build() {
        ChannelPtr channel = newChannel();

        if (serviceInitializer_) {
            channel->setInitializer(serviceInitializer_);
        }

        channel->open();
        channel->pipeline().fireChannelActive();

        return channel;
    }

    const EventLoopPtr& eventLoop() const {
        return eventLoop_;
    }

    const EventLoopPoolPtr& eventLoopPool() const {
        return eventLoopPool_;
    }

private:
    ChannelPtr newChannel() {
        EventLoopPtr loop = eventLoop_;

        if (!loop) {
            loop = eventLoopPool_->nextLoop();
        }

        ChannelPtr channel(new ServiceChannel(loop,
                                             clientInitializer_,
                                             connections_));

        clientChannels_.insert(std::make_pair(channel->id(), channel));
        return channel;
    }

private:
    typedef std::map<int, ChannelPtr> ClientChannels;

private:
    Connections connections_;

    EventLoopPtr eventLoop_;
    EventLoopPoolPtr eventLoopPool_;

    Channel::Initializer clientInitializer_;
    Channel::Initializer serviceInitializer_;

    ClientChannels clientChannels_;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_BUILDER_CLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
