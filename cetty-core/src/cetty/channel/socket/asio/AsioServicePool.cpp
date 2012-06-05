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

#include <cetty/channel/socket/asio/AsioServicePool.h>

#include <boost/bind.hpp>

#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

    using namespace cetty::logging;
    using namespace boost::asio;

    InternalLogger* AsioServicePool::logger = NULL;

AsioServicePool::AsioServicePool(int poolSize, bool onlyMainThread)
    : usingthread(!onlyMainThread),
      running(false),
      nextServiceIndex(0),
      mainThreadId(boost::this_thread::get_id()) {
    
          if (NULL == logger) {
              logger = InternalLoggerFactory::getInstance("AsioServicePool");
          }

    if (poolSize <= 0) {
        poolSize = boost::thread::hardware_concurrency();
        LOG_WARN(logger, "poolSize is negative, instead of the cpu number : %d.", poolSize);
    }
    else if (onlyMainThread && poolSize != 1) {
        LOG_WARN(logger, "if set onlyMainThread, pool size only can be 1.");
        poolSize = 1;
    }
    this->poolSize = poolSize;

    // Give all the io_services work to do so that their run() functions will not
    // exit until they are explicitly stopped.
    serviceContexts.resize(poolSize);
    for (int i = 0; i < poolSize; ++i) {
        AsioServicePtr service = new AsioService(i);
        serviceContexts[i].service = service;
        serviceContexts[i].work = WorkPtr(new io_service::work(*service));
    }

    // automatic start
    if (usingthread) {
        // Create a pool of threads to run all of the io_services.
        for (std::size_t i = 0; i < serviceContexts.size(); ++i) {
            serviceContexts[i].thread
                = ThreadPtr(new boost::thread(
                                 boost::bind(&AsioServicePool::runIOservice,
                                             this,
                                             boost::ref(serviceContexts[i]))));
        }

        running = true;
    }
    else {
        serviceContexts[0].service->setThreadId(boost::this_thread::get_id());
    }
}

bool AsioServicePool::run() {
    if (!running && !usingthread) {
        LOG_INFO(logger, "AsioServciePool running in main thread mode.");
        if (runIOservice(serviceContexts.front()) < 0) {
            LOG_ERROR(logger, "AsioServicePool run the %d service error.",
                serviceContexts.front().service->getId());
            return false;
        }
    }

    running = true;
    return running;
}

void cetty::channel::socket::asio::AsioServicePool::waitForExit() {
    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; usingthread && i < serviceContexts.size(); ++i) {
        serviceContexts[i].thread->join();
    }
}

void AsioServicePool::stop() {
    // Explicitly stop all io_services.
    for (std::size_t i = 0; i < serviceContexts.size(); ++i) {
        serviceContexts[i].stop();
    }

    running = false;
}

const AsioServicePtr& AsioServicePool::getService() {
    // Only one service.
    if (poolSize == 1) {
        return serviceContexts.front().service;
    }

    // Use a round-robin scheme to choose the next io_service to use.
    AsioServiceContext& context = serviceContexts[nextServiceIndex];
    ++nextServiceIndex;

    if (nextServiceIndex == poolSize) {
        nextServiceIndex = 0;
    }

    return context.service;
}

const AsioServicePtr& AsioServicePool::findService(int id) {
    return serviceContexts.at(id).service;
}

int AsioServicePool::runIOservice(AsioServiceContext& context) {
    BOOST_ASSERT(context.service && "ioservice can not be NULL.");

    context.service->setThreadId(boost::this_thread::get_id());

    boost::system::error_code err;
    std::size_t opCount = context.service->service().run(err);

    // if error happened, try to recover.
    if (err) {
        LOG_ERROR(logger, "when runIOservice, the io service has error = %d.", err.value());
        context.stop();
        return -1;
    }

    LOG_INFO(logger, "runIOservice OK, and %d handlers that were executed.", opCount);
    return opCount;
}

const AsioServicePtr& AsioServicePool::getService(int priority /*= PRIORITY_WORK*/) {
    return getService();
}

}
}
}
}
