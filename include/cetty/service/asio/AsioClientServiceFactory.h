#if !defined(CETTY_SERVICE_ASIO_ASIOCLIENTSERVICEFACTORY_H)
#define CETTY_SERVICE_ASIO_ASIOCLIENTSERVICEFACTORY_H

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

#include <vector>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/util/TimerPtr.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {
class AsioTcpSocketAddressImplFactory;
class AsioIpAddressImplFactory;
}
}
}
}

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace service {
namespace asio {

using namespace cetty::logging;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::util;

class AsioClientServiceFactory;
typedef boost::intrusive_ptr<AsioClientServiceFactory> AsioClientServiceFactoryPtr;

class AsioClientServiceFactory : public cetty::channel::ChannelFactory {
public:
    AsioClientServiceFactory(int threadCnt);
    AsioClientServiceFactory(const EventLoopPtr& eventLoop);
    AsioClientServiceFactory(const EventLoopPoolPtr& eventLoopPool);

    virtual ~AsioClientServiceFactory();

    EventLoopPtr getEventLoop();

    virtual ChannelPtr newChannel(const ChannelPipelinePtr& pipeline);
    
    ChannelPtr newChannel(const ChannelPipelinePtr& pipeline,
        const EventLoopPtr& eventLoop);

    virtual void shutdown();

private:
    void init();
    void deinit();

private:
    static InternalLogger* logger;

private:
    EventLoopPtr eventLoop;
    EventLoopPoolPtr eventLoopPool;

    std::vector<ChannelPtr> channels;

    TimerFactoryPtr timerFactory; // keep the life cycle
    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_ASIO_ASIOCLIENTSERVICEFACTORY_H)

// Local Variables:
// mode: c++
// End:
