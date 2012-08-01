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
#include <boost/mpl/size_t.hpp>
#include <cetty/channel/EventLoop.h>
#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/socket/asio/AsioServicePoolPtr.h>

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::logging;

/**
 *
 *
 */
class AsioService : public cetty::channel::EventLoop {
public:
    AsioService(int index) : poolIndex(index) {}

    //AsioServicePool& servicePool();
    //const AsioServicePool& servicePool() const;

    int getId() const { return poolIndex; }

    boost::asio::io_service& service() { return ioService; }
    operator boost::asio::io_service& () { return ioService; }

    virtual void post(const Functor& handler) {
        ioService.post(handler);
    }

private:
    int                     poolIndex;
    boost::asio::io_service ioService;
};

/**
 * A pool of io_service objects.
 */
class AsioServicePool : public cetty::util::ReferenceCounter<AsioServicePool> {
private:
    class AsioServiceContext;

public:
    class Iterator {
    public:
        typedef std::vector<AsioServiceContext>::iterator ServiceIterator;

    public:
        Iterator(const ServiceIterator& iter) : iter(iter) {}
        Iterator(const Iterator& iter) : iter(iter.iter) {}

        Iterator& operator++() {
            ++iter;
            return *this;
        }

        const AsioServicePtr& operator*() {
            return iter->service;
        }

        bool operator==(const Iterator& iter) {
            return this->iter == iter.iter;
        }

        bool operator!=(const Iterator& iter) {
            return this->iter != iter.iter;
        }

    private:
        ServiceIterator iter;
    };

public:
    static const int PRIORITY_BOSS = 0;
    static const int PRIORITY_WORK = 1;

    static AsioService& current();

public:

    /**
     * Construction of AsioServicePool
     *
     * if multi-thread, will automatically run.
     */
    AsioServicePool(int poolSize, bool onlyMainThread = false);

    ~AsioServicePool() {}

    /**
     * Run all io_service objects in the pool.
     * Usually, you only need to run manually under single-thread mode.
     */
    bool run();

    /**
     *
     */
    void waitForExit();

    /**
     * Stop all io_service objects in the pool.
     */
    void stop();

    /**
     * Get an io_service to use.
     */
    const AsioServicePtr& getService();

    const AsioServicePtr& getService(int priority);

    const AsioServicePtr& findService(int id);

    Iterator begin() {
        return Iterator(serviceContexts.begin());
    }
    Iterator end() {
        return Iterator(serviceContexts.end());
    }

    /**
     *
     */
    bool onlyMainThread() const { return !usingthread; }

    bool empty() const { return !poolSize; }
    int  size() const { return poolSize; }

private:
    typedef boost::shared_ptr<boost::thread> ThreadPtr;
    typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

    class AsioServiceContext {
    public:
        enum {
            INITIALIZED =  0,
            RUNNING     =  1,
            STOPPED     = -1
        };

        int state;
        int priority;

        WorkPtr work; /// The work that keeps the io_services running.
        ThreadPtr thread;
        AsioServicePtr service;

        AsioServiceContext() : priority(0), state(INITIALIZED) {}

        void stop() {
            if (state != STOPPED) {
                state = STOPPED;
                service->service().stop();
            }
        }
    };
private:
    AsioServicePool(const AsioServicePool&);
    AsioServicePool& operator=(const AsioServicePool&);

    int runIOservice(AsioServiceContext& context);

private:
    static InternalLogger* logger;

private:
    // indicated this pool using thread except the program's main thread.
    bool usingthread;

    // io service pool already running.
    bool running;

    // The next io_service to use for a connection.
    int nextServiceIndex;

    int poolSize;

    //
    boost::thread::id mainThreadId;

    std::vector<AsioServiceContext> serviceContexts;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVICEPOOL_H)

// Local Variables:
// mode: c++
// End:
