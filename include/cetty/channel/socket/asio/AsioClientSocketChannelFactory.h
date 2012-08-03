#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELFACTORY_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELFACTORY_H

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

#include <map>
#include <vector>

#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/channel/socket/ClientSocketChannelFactory.h>
#include <cetty/util/TimerPtr.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::util;

class AsioIpAddressImplFactory;
class AsioTcpSocketAddressImplFactory;

/**
 * A {@link ClientSocketChannelFactory} which creates a client-side ASIO-based
 * {@link SocketChannel}.  It utilizes the async I/O mode which was
 * introduced with ASIO to serve many number of concurrent connections
 * efficiently.
 *
 * {@link ClientSocketChannelFactory}是创建基于ASIO的客户端{@link SocketChannel}的工厂类.
 *
 * <h3>How threads work</h3>
 * <p>
 * There is only one type of thread in a {@link AsioClientSocketChannelFactory};
 * one is boss thread and the other is worker thread.
 *
 * <h4>Boss thread</h4>
 * <p>
 * One {@link NioClientSocketChannelFactory} has one boss thread.  It makes
 * a connection attempt on request.  Once a connection attempt succeeds,
 * the boss thread passes the connected {@link Channel} to one of the worker
 * threads that the {@link NioClientSocketChannelFactory} manages.
 *
 * <h4>Worker threads</h4>
 * <p>
 * One {@link NioClientSocketChannelFactory} can have one or more worker
 * threads.  A worker thread performs non-blocking read and write for one or
 * more {@link Channel}s in a non-blocking mode.
 *
 * <h3>Life cycle of threads and graceful shutdown</h3>
 * <p>
 * 所有的线程都是由{@link AsioServicePool}创建。
 * All threads are acquired from the {@link Executor}s which were specified
 * when a {@link NioClientSocketChannelFactory} was created.  A boss thread is
 * acquired from the <tt>bossExecutor</tt>, and worker threads are acquired from
 * the <tt>workerExecutor</tt>.  Therefore, you should make sure the specified
 * {@link Executor}s are able to lend the sufficient number of threads.
 * It is the best bet to specify @link Executors#newCachedThreadPool() a cached thread pool@endlink.
 * <p>
 * Both boss and worker threads are acquired lazily, and then released when
 * there's nothing left to process.  All the related resources such as
 * {@link Selector} are also released when the boss and worker threads are
 * released.  Therefore, to shut down a service gracefully, you should do the
 * following:
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
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */

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

    TimerFactoryPtr timerFactory; // keep the life cycle

    std::vector<ChannelPtr> clientChannels;

    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELFACTORY_H)

// Local Variables:
// mode: c++
// End:
