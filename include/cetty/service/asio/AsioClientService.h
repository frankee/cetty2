#if !defined(CETTY_SERVICE_ASIO_ASIOCLIENTSERVICE_H)
#define CETTY_SERVICE_ASIO_ASIOCLIENTSERVICE_H

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

#include <boost/thread.hpp>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/DefaultChannelConfig.h>
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>
#include <cetty/service/ClientService.h>

namespace cetty {
namespace service {
namespace asio {

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;

class AsioClientService : public cetty::service::ClientService {
public:
    AsioClientService(const ChannelFactoryPtr& factory,
                      const ChannelPipelinePtr& pipeline,
                      const AsioServicePtr& ioService);

    virtual ~AsioClientService() {}

    virtual ChannelConfig& getConfig() { return this->config; }
    virtual const ChannelConfig& getConfig() const { return this->config; }

    const AsioServicePtr& getService() {
        return ioService;
    }

    virtual const SocketAddress& getLocalAddress() const { return SocketAddress::NULL_ADDRESS ; }
    virtual const SocketAddress& getRemoteAddress() const { return SocketAddress::NULL_ADDRESS; }
    
    virtual bool isOpen() const { return true; }
    virtual bool isBound() const { return true; }
    virtual bool isConnected() const { return true; }
/*
    virtual int getInterestOps() const;
    */
    //virtual ChannelFuturePtr write(const ChannelMessage& message);


    /*
    virtual ChannelFuturePtr unbind();
    virtual ChannelFuturePtr close();
    virtual ChannelFuturePtr disconnect();
    virtual ChannelFuturePtr setInterestOps(int interestOps);

    virtual bool setClosed();
    */

protected:
    static InternalLogger* logger;

protected:
    boost::thread::id threadId;
    AsioServicePtr  ioService;

    DefaultChannelConfig config;
};
}
}
}

#endif //#if !defined(CETTY_SERVICE_ASIO_ASIOCLIENTSERVICE_H)

// Local Variables:
// mode: c++
// End:
