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

#include <cetty/channel/asio/AsioServicePool.h>

#include <boost/bind.hpp>

#include <cetty/Types.h>
#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/channel/asio/AsioService.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::logging;
using namespace boost::asio;

class AsioServiceHolder : public EventLoopPool::EventLoopHolder {
public:
    enum {
        INITIALIZED =  0,
        RUNNING     =  1,
        STOPPED     = -1
    };

    typedef boost::shared_ptr<boost::thread> ThreadPtr;
    typedef boost::shared_ptr<boost::asio::io_service::work> WorkPtr;

public:
    AsioServiceHolder(const AsioServicePtr& service)
        : state_(INITIALIZED),
          priority_(0),
          work_(new io_service::work(service->service())),
          service_(service),
          eventLoop_(boost::static_pointer_cast<EventLoop>(service)) {
    }

    virtual ~AsioServiceHolder() {
    }

    virtual const EventLoopPtr& eventLoop() const {
        return eventLoop_;
    }

    const AsioServicePtr& service() const {
        return service_;
    }

    void setThread(const ThreadPtr& thread) {
        thread_ = thread;
    }

    void waitingForStop() {
        if (thread_) {
            thread_->join();
        }
    }

    void stop() {
        if (state_ != STOPPED && service_) {
            state_ = STOPPED;
            service_->stop();
        }
    }

private:
    int state_;
    int priority_;

    WorkPtr work_; /// The work that keeps the io_services running.
    ThreadPtr thread_;
    AsioServicePtr service_;
    EventLoopPtr eventLoop_;
};

AsioServicePool::AsioServicePool(int threadCnt)
    : EventLoopPool(threadCnt),
      nextServiceIndex_(0) {

    // Give all the io_services work to do so that their run() functions will not
    // exit until they are explicitly stopped.
    for (int i = 0; i < size(); ++i) {
        appendLoopHolder(new AsioServiceHolder(
                             new AsioService(shared_from_this())));
    }

    // automatic start
    if (!isSingleThread()) {
        LOG_INFO << "automatic start the " << size() << " event loops in thread";

        // Create a pool of threads to run all of the io_services.
        for (std::size_t i = 0; i < size(); ++i) {
            AsioServiceHolder* holder =
                down_cast<AsioServiceHolder*>(loopHolderAt(i));

            holder->setThread(
                ThreadPtr(new boost::thread(
                              boost::bind(&AsioServicePool::runIOservice,
                                          this,
                                          holder))));
        }

        setStarted(true);
    }
    else {
        LOG_INFO << "the event loop pool is main thread mode, will start later.";

        AsioServiceHolder* holder =
            down_cast<AsioServiceHolder*>(loopHolderAt(0));

        ThreadId id = CurrentThread::id();
        holder->eventLoop()->setThreadId(id);
        insertLoop(id, holder->eventLoop());
    }
}

bool AsioServicePool::start() {
    if (!isStarted() && isSingleThread()) {
        LOG_INFO << "start the AsioServciePool in main thread mode.";

        if (runIOservice(down_cast<AsioServiceHolder*>(loopHolderAt(0))) < 0) {
            LOG_ERROR << "AsioServicePool run the main thread service error.";
            return false;
        }
    }

    setStarted(true);
    return true;
}

void AsioServicePool::waitingForStop() {
    if (isSingleThread()) {
        return;
    }

    // Wait for all threads in the pool to exit.
    for (int i = 0; i < size(); ++i) {
        AsioServiceHolder* holder =
            down_cast<AsioServiceHolder*>(loopHolderAt(i));

        holder->waitingForStop();
    }
}

void AsioServicePool::stop() {
    // Explicitly stop all io_services.
    for (int i = 0; i < size(); ++i) {
        AsioServiceHolder* holder =
            down_cast<AsioServiceHolder*>(loopHolderAt(i));

        holder->stop();
    }

    setStarted(false);
}

const AsioServicePtr& AsioServicePool::nextService() {
    return nextServiceHolder()->service();
}

int AsioServicePool::runIOservice(AsioServiceHolder* holder) {
    BOOST_ASSERT(holder && holder->service() && "io service can not be NULL.");

    const AsioServicePtr& service = holder->service();

    ThreadId id = CurrentThread::id();
    service->setThreadId(id);

    {
        boost::lock_guard<boost::mutex> lock(mutext_);
        insertLoop(id, holder->eventLoop());
    }

    boost::system::error_code err;
    std::size_t opCount = service->service().run(err);

    // if error happened, try to recover.
    if (err) {
        LOG_ERROR  << "when runIOservice, the io service has error = " << err.value();
        stop();
        return -1;
    }

    LOG_INFO << "runIOservice completed, and " << opCount << "handlers that were executed.";
    return opCount;
}

const EventLoopPtr& AsioServicePool::nextLoop() {
    return nextServiceHolder()->eventLoop();
}

AsioServiceHolder* AsioServicePool::nextServiceHolder() {
    // Only one service.
    if (size() == 1) {
        return down_cast<AsioServiceHolder*>(loopHolderAt(0));
    }

    // Use a round-robin scheme to choose the next io_service to use.
    AsioServiceHolder* holder =
        down_cast<AsioServiceHolder*>(loopHolderAt(nextServiceIndex_));

    ++nextServiceIndex_;

    if (nextServiceIndex_ == size()) {
        nextServiceIndex_ = 0;
    }

    return holder;
}

}
}
}
