#if !defined(CETTY_BOOTSTRAP_ASIO_ASIOSERVERBOOTSTRAP_H)
#define CETTY_BOOTSTRAP_ASIO_ASIOSERVERBOOTSTRAP_H

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

#include <cetty/bootstrap/ServerBootstrap.h>
#include <cetty/channel/asio/AsioSocketAddressImplFactory.h>
#include <cetty/channel/asio/AsioIpAddressImplFactory.h>


namespace cetty {
namespace bootstrap {
namespace asio {

using namespace cetty::channel::asio;

class AsioServerBootstrap : public ServerBootstrap {
public:
    /**
     * Creates a new instance.
     *
     * @throws IOException
     *         if catch the exception, when open the acceptor.
     */
    AsioServerBootstrap(int parentThreadCnt, int childThreadCnt = 0);

    AsioServerBootstrap(const EventLoopPoolPtr& pool);

    AsioServerBootstrap(const EventLoopPoolPtr& parentPool,
                        const EventLoopPoolPtr& childPool);

    virtual ~AsioServerBootstrap();

    virtual void shutdown();

protected:
    virtual ChannelPtr newChannel();

private:
    void init();
    void deinit();

private:
    EventLoopPoolPtr parentPool;
    EventLoopPoolPtr childPool;

    std::vector<ChannelPtr> serverChannels;

    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};

}
}
}


#endif //#if !defined(CETTY_BOOTSTRAP_ASIO_ASIOSERVERBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
