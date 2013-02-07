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
    AsioServiceHolder() : state(INITIALIZED), priority(0) {}
    virtual ~AsioServiceHolder() {}

    virtual const EventLoopPtr& getEventLoop() const {
        return eventLoop;
    }

    int state;
    int priority;

    WorkPtr work; /// The work that keeps the io_services running.
    ThreadPtr thread;
    AsioServicePtr service;
    EventLoopPtr eventLoop;

    void stop() {
        if (state != STOPPED) {
            state = STOPPED;
            service->stop();
        }
    }
};

AsioServicePool::AsioServicePool(int threadCnt)
    : EventLoopPool(threadCnt),
      nextServiceIndex_(0) {

    // Give all the io_services work to do so that their run() functions will not
    // exit until they are explicitly stopped.
    for (int i = 0; i < eventLoopCnt_; ++i) {
        AsioServiceHolder* holder = new AsioServiceHolder();
        AsioServicePtr service = new AsioService(shared_from_this());
        holder->service = service;
        holder->eventLoop = boost::static_pointer_cast<EventLoop>(service);
        holder->work = WorkPtr(new io_service::work(service->service()));

        eventLoops_.push_back(holder);
    }

    // automatic start
    if (!mainThread_) {
        // Create a pool of threads to run all of the io_services.
        for (std::size_t i = 0; i < eventLoops_.size(); ++i) {
            AsioServiceHolder* holder = (AsioServiceHolder*)eventLoops_[i];
            holder->thread
                = ThreadPtr(new boost::thread(
                                boost::bind(&AsioServicePool::runIOservice,
                                            this,
                                            holder)));
        }

        started_ = true;
    }
    else {
        AsioServiceHolder* holder = (AsioServiceHolder*)eventLoops_.front();

        ThreadId id = CurrentThread::id();
        holder->service->setThreadId(id);
        allEventLoops_.insert(std::make_pair(id, holder->eventLoop));
    }
}

bool AsioServicePool::start() {
    if (!started_ && mainThread_) {
        LOG_INFO << "AsioServciePool running in main thread mode.";

        if (runIOservice((AsioServiceHolder*)eventLoops_.front()) < 0) {
            LOG_ERROR << "AsioServicePool run the main thread service error.";
            return false;
        }
    }

    started_ = true;
    return true;
}

void AsioServicePool::waitForStop() {
    if (mainThread_) {
        return;
    }

    // Wait for all threads in the pool to exit.
    for (int i = 0; i < eventLoopCnt_; ++i) {
        AsioServiceHolder* holder = (AsioServiceHolder*)eventLoops_[i];
        holder->thread->join();
    }
}

void AsioServicePool::stop() {
    // Explicitly stop all io_services.
    for (int i = 0; i < eventLoopCnt_; ++i) {
        AsioServiceHolder* holder = (AsioServiceHolder*)eventLoops_[i];
        holder->stop();
    }

    started_ = false;
}

const AsioServicePtr& AsioServicePool::getNextService() {
    return getNextServiceHolder()->service;
}

int AsioServicePool::runIOservice(AsioServiceHolder* holder) {
    BOOST_ASSERT(holder && holder->service && "io service can not be NULL.");

    AsioServicePtr& service = holder->service;

    ThreadId id = CurrentThread::id();
    service->setThreadId(id);
    
    {
        boost::lock_guard<boost::mutex> lock(mutext_);
        allEventLoops_.insert(std::make_pair(id, holder->eventLoop));
    }

    boost::system::error_code err;
    std::size_t opCount = service->service().run(err);

    // if error happened, try to recover.
    if (err) {
        LOG_ERROR  << "when runIOservice, the io service has error = " << err.value();
        stop();
        return -1;
    }

    LOG_INFO << "runIOservice OK, and " << opCount << "handlers that were executed.";
    return opCount;
}

const EventLoopPtr& AsioServicePool::getNextLoop() {
    return getNextServiceHolder()->eventLoop;
}

AsioServiceHolder* AsioServicePool::getNextServiceHolder() {
    // Only one service.
    if (eventLoopCnt_ == 1) {
        return (AsioServiceHolder*)eventLoops_.front();
    }

    // Use a round-robin scheme to choose the next io_service to use.
    AsioServiceHolder* holder = (AsioServiceHolder*)eventLoops_[nextServiceIndex_];
    ++nextServiceIndex_;

    if (nextServiceIndex_ == eventLoopCnt_) {
        nextServiceIndex_ = 0;
    }

    return holder;
}

}
}
}
