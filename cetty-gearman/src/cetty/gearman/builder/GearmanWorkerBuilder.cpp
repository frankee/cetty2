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

#include <cetty/gearman/builder/GearmanWorkerBuilder.h>

#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/gearman/GearmanWorker.h>
#include <cetty/gearman/GearmanWorkerHandler.h>
#include <cetty/gearman/protocol/GearmanMessageCodec.h>

namespace cetty {
namespace gearman {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::gearman::protocol;

GearmanWorkerBuilder::GearmanWorkerBuilder() {
}

GearmanWorkerBuilder::GearmanWorkerBuilder(int threadCnt)
    : pool_(new AsioServicePool(threadCnt)) {
}

GearmanWorkerBuilder& GearmanWorkerBuilder::buildWorkers() {
    if (pool_) {
        AsioServicePool::Iterator itr = pool_->begin();

        for (; itr != pool_->end(); ++itr) {
            buildWorker(*itr);
        }
    }

    return *this;
}

void GearmanWorkerBuilder::buildWorker(const EventLoopPtr& eventLoop) {
    //to connect to remote at this point
    GearmanWorkerPtr worker =
        new GearmanWorker(eventLoop,
                          boost::bind(&GearmanWorkerBuilder::initializeChannel,
                                      this,
                                      _1),
                          connections_);

    workers_.push_back(worker);
}

GearmanWorkerBuilder& GearmanWorkerBuilder::registerWorker(const std::string& functionName,
        const WorkFunctor& worker) {
    workFunctors_[functionName] = worker;
    return *this;
}

bool GearmanWorkerBuilder::initializeChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<LengthFieldBasedFrameDecoder::HandlerPtr>("frameDecoder",
            LengthFieldBasedFrameDecoder::HandlerPtr(
                new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 8, 4, 0, 4)));

    pipeline.addLast<GearmanMessageCodec::HandlerPtr>("gearmanCodec",
            GearmanMessageCodec::HandlerPtr(new GearmanMessageCodec));

    GearmanWorkerHandler::HandlerPtr worker(new GearmanWorkerHandler);
    WorkFunctors::const_iterator itr = workFunctors_.begin();

    for (; itr != workFunctors_.end(); ++itr) {
        worker->registerWorker(itr->first, itr->second);
    }

    pipeline.addLast<GearmanWorkerHandler::HandlerPtr>("gearmanWorker",
            worker);

    if (additionalInitializer_) {
        return additionalInitializer_(channel);
    }

    return true;
}

}
}
}
