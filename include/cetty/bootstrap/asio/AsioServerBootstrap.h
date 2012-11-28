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
namespace cetty { namespace bootstrap { namespace asio { 
    class AsioServerSocketChannelFactory : public ServerSocketChannelFactory {
public:
    /**
     * Creates a new instance.
     *
     * @throws IOException
     *         if catch the exception, when open the acceptor.
     */
    AsioServerSocketChannelFactory(int parentThreadCnt, int childThreadCnt = 0);
    AsioServerSocketChannelFactory(const EventLoopPoolPtr& pool);
    AsioServerSocketChannelFactory(const EventLoopPoolPtr& parentPool,
                                   const EventLoopPoolPtr& childPool);
    virtual ~AsioServerSocketChannelFactory();

    virtual ChannelPtr newChannel(const ChannelPipelinePtr& pipeline);
    virtual void shutdown();

    virtual void setChildChannelPipeline(const ChannelPipelinePtr& pipeline);
    virtual const ChannelPipelinePtr& getChildChannelPipeline() const;

private:
    void init();
    void deinit();

private:
    EventLoopPoolPtr parentPool;
    EventLoopPoolPtr childPool;

    ChannelPipelinePtr childPipeline;
    std::vector<ChannelPtr> serverChannels;

    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};

    AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(
        const EventLoopPoolPtr& pool)
        : parentPool(pool),
        childPool(pool),
        socketAddressFactory(),
        ipAddressFactory() {
            BOOST_ASSERT(pool && "ioServicePool SHOULD NOT BE NULL.");
            init();
    }

    AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(
        int parentThreadCnt,
        int childThreadCnt /*= 0*/)
        : socketAddressFactory(),
        ipAddressFactory() {

            parentPool = new AsioServicePool(parentThreadCnt);

            if (0 == childThreadCnt) {
                childPool = parentPool;
            }
            else {
                childPool = new AsioServicePool(childThreadCnt);
            }

            BOOST_ASSERT(parentPool && childPool && "ioServicePool SHOULD NOT BE NULL.");
            init();
    }

    AsioServerSocketChannelFactory::AsioServerSocketChannelFactory(
        const EventLoopPoolPtr& parentPool,
        const EventLoopPoolPtr& childPool)
        : parentPool(parentPool),
        childPool(childPool),
        socketAddressFactory(),
        ipAddressFactory() {

            BOOST_ASSERT(parentPool && childPool && "ioServicePool SHOULD NOT BE NULL.");
            init();
    }

    AsioServerSocketChannelFactory::~AsioServerSocketChannelFactory() {
        deinit();
    }

    ChannelPtr AsioServerSocketChannelFactory::newChannel(const ChannelPipelinePtr& pipeline) {
        ChannelPtr channel =
            new AsioServerSocketChannel(parentPool->getNextLoop(),
            shared_from_this(),
            pipeline,
            childPipeline,
            childPool);

        if (channel->isOpen()) {
            LOG_INFO << "Created the AsioServerSocketChannel, firing the Channel Created Event.";
            channel->pipeline()->fireChannelOpen();

            serverChannels.push_back(channel);
        }
        else {
            channel.reset();
        }

        return channel;
    }

    void AsioServerSocketChannelFactory::shutdown() {
        childPool->stop();
        childPool->waitForStop();

        parentPool->stop();
        parentPool->waitForStop();

        serverChannels.clear();
    }

    void AsioServerSocketChannelFactory::init() {
        if (!SocketAddress::hasFactory()) {
            EventLoopPtr loop = parentPool->getNextLoop();
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

    void AsioServerSocketChannelFactory::deinit() {
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

    void AsioServerSocketChannelFactory::setChildChannelPipeline(
        const ChannelPipelinePtr& pipeline) {
            this->childPipeline = pipeline;
    }

    const ChannelPipelinePtr& AsioServerSocketChannelFactory::getChildChannelPipeline() const {
        return this->childPipeline;
    }

}}}


#endif //#if !defined(CETTY_BOOTSTRAP_ASIO_ASIOSERVERBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
