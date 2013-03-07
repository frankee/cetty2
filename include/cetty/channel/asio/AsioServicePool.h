#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICEPOOL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICEPOOL_H

//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/asio/AsioServicePoolPtr.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;

class AsioServiceHolder;

/**
 * A pool of io_service objects.
 */
class AsioServicePool : public cetty::channel::EventLoopPool {
public:
    /**
     * Construction of AsioServicePool
     *
     * if multi-thread, will automatically run.
     */
    AsioServicePool(int threadCnt);

    virtual ~AsioServicePool() {}

    /**
     * Run all io_service objects in the pool.
     * Usually, you only need to run manually under single-thread mode.
     */
    virtual bool start();

    /**
     * Stop all io_service objects in the pool.
     */
    virtual void stop();

    /**
     *
     */
    virtual void waitingForStop();

    virtual const EventLoopPtr& nextLoop();

    /**
     * Get an io_service to use.
     */
    const AsioServicePtr& nextService();

private:
    AsioServicePool(const AsioServicePool&);
    AsioServicePool& operator=(const AsioServicePool&);

    int runIOservice(AsioServiceHolder* holder);
    AsioServiceHolder* nextServiceHolder();

private:
    typedef boost::shared_ptr<boost::thread> ThreadPtr;
    typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

private:
    // The next io_service to use for a connection.
    int nextServiceIndex_;

    boost::mutex mutext_;
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICEPOOL_H)

// Local Variables:
// mode: c++
// End:
