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

#include <cetty/bootstrap/ClientBootstrap.h>

namespace cetty {
namespace bootstrap {
namespace asio {

class AsioClientBootstrap : public ClientBootstrap {
public:


    virtual newChannel() {

    }
};

class AsioClientSocketChannelFactory
        : public cetty::channel::socket::SocketChannelFactory {
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
    AsioClientSocketChannelFactory(int threadCnt);
    AsioClientSocketChannelFactory(const EventLoopPtr& eventLoop);
    AsioClientSocketChannelFactory(const EventLoopPoolPtr& eventLoopPool);

    virtual ~AsioClientSocketChannelFactory();

    virtual ChannelPtr newChannel(const ChannelPipelinePtr& pipeline);

    virtual void shutdown();

private:
    void init();
    void deinit();

private:
    typedef std::map<int, ChannelPtr> ClientChannels;

private:
    EventLoopPtr eventLoop;
    EventLoopPoolPtr eventLoopPool;

    std::vector<ChannelPtr> clientChannels;

    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};


AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(
    const EventLoopPtr& eventLoop)
    : eventLoop(eventLoop),
    eventLoopPool(),
    socketAddressFactory(NULL),
    ipAddressFactory(NULL) {
        init();
}

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(
    const EventLoopPoolPtr& eventLoopPool)
    : eventLoop(),
    eventLoopPool(eventLoopPool),
    socketAddressFactory(NULL),
    ipAddressFactory(NULL) {
        init();
}

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(int threadCnt)
    : eventLoopPool(new AsioServicePool(threadCnt)),
    socketAddressFactory(NULL),
    ipAddressFactory(NULL) {
        init();
}

AsioClientSocketChannelFactory::~AsioClientSocketChannelFactory() {
    deinit();
}

void AsioClientSocketChannelFactory::init() {
    if (!SocketAddress::hasFactory()) {
        EventLoopPtr loop
            = eventLoopPool ? eventLoopPool->getNextLoop() : eventLoop;
        AsioServicePtr service = boost::dynamic_pointer_cast<AsioService>(loop);
        BOOST_ASSERT(service && "AsioClientSocketChannelFactory only can init with AsioService");

        socketAddressFactory =
            new AsioTcpSocketAddressImplFactory(service->service());

        SocketAddress::setFacotry(socketAddressFactory);
    }

    if (!IpAddress::hasFactory()) {
        ipAddressFactory = new AsioIpAddressImplFactory();
        IpAddress::setFactory(ipAddressFactory);
    }
}

void AsioClientSocketChannelFactory::deinit() {
    if (socketAddressFactory) {
        SocketAddress::resetFactory();

        delete socketAddressFactory;
        socketAddressFactory = NULL;
    }

    if (ipAddressFactory) {
        IpAddress::resetFactory();

        delete ipAddressFactory;
        ipAddressFactory = NULL;
    }
}

ChannelPtr AsioClientSocketChannelFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    const EventLoopPtr& eventLoop = eventLoopPool ?
        eventLoopPool->getNextLoop()
        : this->eventLoop;

    ChannelPtr client =
        new AsioSocketChannel(eventLoop,
        shared_from_this(),
        pipeline);

    LOG_INFO << "AsioSocketChannel firing the Channel Created Event.";
    client->pipeline()->fireChannelOpen();

    clientChannels.push_back(client);

    return client;
}

void AsioClientSocketChannelFactory::shutdown() {
    if (eventLoopPool) {
        eventLoopPool->stop();
        eventLoopPool->waitForStop();
    }

    clientChannels.clear();
}

}
}
}

#endif //#if !defined(CETTY_BOOTSTRAP_ASIO_ASIOCLIENTBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
