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

#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h>
#include <cetty/gearman/GearmanWorker.h>
#include <cetty/gearman/GearmanDecoder.h>
#include <cetty/gearman/GearmanEncoder.h>
#include <cetty/gearman/GearmanWorkerHandler.h>

namespace cetty {
namespace gearman {
namespace builder {

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::codec::frame;

GearmanWorkerBuilder::GearmanWorkerBuilder()
    : ServerBuilder() {

}

GearmanWorkerBuilder::GearmanWorkerBuilder(int threadCnt)
    : ServerBuilder(threadCnt) {

}

GearmanWorkerBuilder::~GearmanWorkerBuilder() {

}

void GearmanWorkerBuilder::addConnection(const std::string& host, int port) {
    connections.push_back(Connection(host, port, 1));
}

const std::vector<GearmanWorkerPtr>& GearmanWorkerBuilder::buildWorkers() {
    const AsioServicePoolPtr& pool = getServicePool();

    if (pool) {
        AsioServicePool::Iterator itr = pool->begin();

        for (; itr != pool->end(); ++itr) {
            buildWorker(*itr);
        }
    }

    return workers;
}

void GearmanWorkerBuilder::buildWorker(const AsioServicePtr& ioService) {
    GearmanWorkerPtr worker =
        new GearmanWorker(ioService, pipeline, connections);
    workers.push_back(worker);
}

void GearmanWorkerBuilder::setWorkerPipeline(const ChannelPipelinePtr& pipeline) {
    if (pipeline && this->pipeline != pipeline) {
        this->pipeline = pipeline;
    }
}

const ChannelPipelinePtr& GearmanWorkerBuilder::getWorkerPipeline() {
    return this->pipeline;
}

void GearmanWorkerBuilder::initDefaultPipeline() {
    pipeline = Channels::pipeline();

    pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 0, 4, 0, 4));

    pipeline->addLast("gearmanDecoder", new GearmanDecoder());
    pipeline->addLast("gearmanEncoder", new GearmanEncoder());
    pipeline->addLast("gearmanWorker", new GearmanWorkerHandler());
}

void GearmanWorkerBuilder::registerWorker(const std::string& functionName,
        const WorkerFuncotr& worker) {

    ChannelHandlerPtr handler = pipeline->get("gearmanWorker");
    GearmanWorkerHandlerPtr workerHandler =
        boost::dynamic_pointer_cast<GearmanWorkerHandler>(handler);

    if (workerHandler) {
        workerHandler->registerWorker(functionName, worker);
    }
}

}
}
}
