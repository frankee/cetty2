#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNELFACTORY_H)

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

#include <vector>
#include <cetty/channel/socket/ServerSocketChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>
#include <cetty/util/TimerFwd.h>

namespace cetty {
    namespace logging {
        class InternalLogger;
    }
}

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::channel::socket;
using namespace cetty::util;
using namespace cetty::logging;

class AsioTcpSocketAddressImplFactory;
class AsioIpAddressImplFactory;

/**
 * A {@link ServerSocketChannelFactory} which creates a server-side NIO-based
 * {@link ServerSocketChannel}.  It utilizes the non-blocking I/O mode which
 * was introduced with NIO to serve many number of concurrent connections
 * efficiently.
 *
 * <h3>How threads work</h3>
 * <p>
 * There are two types of threads in a {@link NioServerSocketChannelFactory};
 * one is boss thread and the other is worker thread.
 *
 * <h4>Boss threads</h4>
 * <p>
 * Each bound {@link ServerSocketChannel} has its own boss thread.
 * For example, if you opened two server ports such as 80 and 443, you will
 * have two boss threads.  A boss thread accepts incoming connections until
 * the port is unbound.  Once a connection is accepted successfully, the boss
 * thread passes the accepted {@link Channel} to one of the worker
 * threads that the {@link NioServerSocketChannelFactory} manages.
 *
 * <h4>Worker threads</h4>
 * <p>
 * One {@link NioServerSocketChannelFactory} can have one or more worker
 * threads.  A worker thread performs non-blocking read and write for one or
 * more {@link Channel}s in a non-blocking mode.
 *
 * <h3>Life cycle of threads and graceful shutdown</h3>
 * <p>
 * All threads are acquired from the {@link Executor}s which were specified
 * when a {@link NioServerSocketChannelFactory} was created.  Boss threads are
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
 * <li>unbind all channels created by the factory,
 * <li>close all child channels accepted by the unbound channels, and
 *     (these two steps so far is usually done using {@link ChannelGroup#close()})</li>
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

class AsioServerSocketChannelFactory : public ServerSocketChannelFactory {
public:
    /**
     * Creates a new instance.
     *
     * @throws IOException
     *         if catch the exception, when open the acceptor.
     */
    AsioServerSocketChannelFactory(int threadCnt);
    AsioServerSocketChannelFactory(const AsioServicePoolPtr& pool);
    virtual ~AsioServerSocketChannelFactory();

    virtual ChannelPtr newChannel(const ChannelPipelinePtr& pipeline);
    virtual void releaseExternalResources();

private:
    void init();
    void deinit();

private:
    static InternalLogger* logger;

private:
    AsioServicePoolPtr ioServicePool;

    TimerFactoryPtr timerFactory; // keep the life cycle.

    AsioTcpSocketAddressImplFactory* socketAddressFactory;
    AsioIpAddressImplFactory* ipAddressFactory;

    std::vector<ChannelPtr> channels;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNELFACTORY_H)

// Local Variables:
// mode: c++
// End:

