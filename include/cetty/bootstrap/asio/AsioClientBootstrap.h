#if !defined(CETTY_BOOTSTRAP_ASIO_ASIOCLIENTBOOTSTRAP_H)
#define CETTY_BOOTSTRAP_ASIO_ASIOCLIENTBOOTSTRAP_H

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

#include <cetty/channel/asio/AsioServicePtr.h>
#include <cetty/channel/asio/AsioServicePoolPtr.h>

#include <cetty/bootstrap/ClientBootstrap.h>

#include <cetty/channel/asio/AsioSocketAddressImplFactory.h>
#include <cetty/channel/asio/AsioIpAddressImplFactory.h>

namespace cetty {
namespace bootstrap {
namespace asio {

using namespace cetty::channel::asio;

class AsioClientBootstrap : public ClientBootstrap {
public:
    /**
     * Creates a new instance.  Calling this constructor is same with calling
     * {@link #NioClientSocketChannelFactory(Executor, Executor, int)} with 2 *
     * the number of available processors in the machine.  The number of
     * available processors is obtained by {@link Runtime#availableProcessors()}.
     *
     * @param bossExecutor
     *        the {@link Executor} which will execute the boss thread
     * @param workerExecutor
     *        the {@link Executor} which will execute the I/O worker threads
     * @param workerCount
     *        the maximum number of I/O worker threads
     */
    AsioClientBootstrap(int threadCnt);
    AsioClientBootstrap(const EventLoopPtr& eventLoop);
    AsioClientBootstrap(const EventLoopPoolPtr& eventLoopPool);

    virtual ~AsioClientBootstrap();

    virtual ChannelPtr newChannel();

private:
    void init();
    void deinit();

private:
    EventLoopPtr eventLoop_;

    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};

}
}
}

#endif //#if !defined(CETTY_BOOTSTRAP_ASIO_ASIOCLIENTBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
