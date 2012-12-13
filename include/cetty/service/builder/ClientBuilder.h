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

template<typename ReqT, typename RepT = ReqT>
class ClientBuilder {
public:
    typedef ClientBuilder<ReqT, RepT> Self;
    typedef ClientService<ReqT, RepT> ClientServiceChannel;

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

    void setChannelInitializer(const Channel::Initializer& initializer) {
        clientInitializer_ = initializer;
    }

    void addConnection(const std::string& host, int port, int limit = 1) {
        connections_.push_back(Connection(host, port, limit));
    }

    ChannelPtr build() {
        ChannelPtr channel = newChannel();
        channel->setInitializer(
            boost::bind(&Self::initilizeClientService, this, _1));

        channel->initialize();
        channel->pipeline().fireChannelActive();

        return channel;
    }

private:
    bool initilizeClientService(const ChannelPtr& ch) {
        return true;
    }

private:
    ChannelPtr newChannel() {
        EventLoopPtr loop = eventLoop_;

        if (!loop) {
            loop = eventLoopPool_->getNextLoop();
        }

        return ChannelPtr(new ClientServiceChannel(loop,
                          clientInitializer_,
                          connections_));
    }

private:
    Connections connections_;

    EventLoopPtr eventLoop_;
    EventLoopPoolPtr eventLoopPool_;

    Channel::Initializer clientInitializer_;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_BUILDER_CLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
