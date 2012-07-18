#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNELFACTORY_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNELFACTORY_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <boost/asio.hpp>
#include <cetty/channel/socket/DatagramChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/util/TimerFactory.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::util;

class AsioDatagramPipelineSink;
class AsioIpAddressImplFactory;
class AsioUdpSocketAddressImplFactory;

/**
 * A {@link DatagramChannelFactory} that creates a NIO-based connectionless
 * {@link DatagramChannel}. It utilizes the non-blocking I/O mode which
 * was introduced with NIO to serve many number of concurrent connections
 * efficiently.
 *
 * <h3>How threads work</h3>
 * <p>
 * There is only one thread type in a {@link NioDatagramChannelFactory};
 * worker threads.
 *
 * <h4>Worker threads</h4>
 * <p>
 * One {@link NioDatagramChannelFactory} can have one or more worker
 * threads.  A worker thread performs non-blocking read and write for one or
 * more {@link DatagramChannel}s in a non-blocking mode.
 *
 * <h3>Life cycle of threads and graceful shutdown</h3>
 * <p>
 * All worker threads are acquired from the {@link Executor} which was specified
 * when a {@link NioDatagramChannelFactory} was created.  Therefore, you should
 * make sure the specified {@link Executor} is able to lend the sufficient
 * number of threads.  It is the best bet to specify
 * @link Executors#newCachedThreadPool() a cached thread pool@endlink.
 * <p>
 * All worker threads are acquired lazily, and then released when there's
 * nothing left to process.  All the related resources such as {@link Selector}
 * are also released when the worker threads are released.  Therefore, to shut
 * down a service gracefully, you should do the following:
 *
 * <ol>
 * <li>close all channels created by the factory usually using
 *     {@link ChannelGroup#close()}, and</li>
 * <li>call {@link #releaseExternalResources()}.</li>
 * </ol>
 *
 * Please make sure not to shut down the executor until all channels are
 * closed.  Otherwise, you will end up with a {@link RejectedExecutionException}
 * and the related resources might not be released properly.
 *
 * <h3>Limitation</h3>
 * <p>
 * Multicast is not supported.  Please use {@link OioDatagramChannelFactory}
 * instead.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */
class AsioDatagramChannelFactory : public cetty::channel::socket::DatagramChannelFactory {
public:
    /**
     * Creates a new instance.  Calling this constructor is same with calling
     * {@link #NioDatagramChannelFactory(Executor, int)} with 2 * the number of
     * available processors in the machine.  The number of available processors
     * is obtained by {@link Runtime#availableProcessors()}.
     *
     * @param workerExecutor
     *        the {@link Executor} which will execute the I/O worker threads
     */
    AsioDatagramChannelFactory(int ioThreadCount = 1);
    virtual ~AsioDatagramChannelFactory();

    virtual Channel* newChannel(cetty::channel::ChannelPipeline* pipeline);

    virtual int  getIpProtocolVersion() const { return ipProtocol; }
    virtual void setIpProtocolVersion(int version) { ipProtocol = version; }

    virtual void releaseExternalResources();

    AsioServicePoolPtr& getIOServicePool() { return ioServicePool; }

    void start();

private:
    void createSocketAddressImplFactory();
    void destorySocketAddressImplFactory();

private:
    bool started;
    int ioThreadCount;
    int ipProtocol;

    AsioDatagramPipelineSink* sink;

    AsioServicePoolPtr ioServicePool;
    TimerFactoryPtr timerFactory;

    AsioUdpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIODATAGRAMCHANNELFACTORY_H)

// Local Variables:
// mode: c++
// End:
