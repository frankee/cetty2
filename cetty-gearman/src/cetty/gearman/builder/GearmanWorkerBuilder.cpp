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
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/gearman/GearmanWorker.h>
#include <cetty/gearman/GearmanDecoder.h>
#include <cetty/gearman/GearmanEncoder.h>
#include <cetty/gearman/GearmanWorkerHandler.h>

namespace cetty {
namespace gearman {
namespace builder {

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::codec;

GearmanWorkerBuilder::GearmanWorkerBuilder()
    : ServerBuilder() {
}

GearmanWorkerBuilder::GearmanWorkerBuilder(int threadCnt)
    : ServerBuilder(threadCnt), pipeline() {
}

GearmanWorkerBuilder::~GearmanWorkerBuilder() {

}

void GearmanWorkerBuilder::addConnection(const std::string& host, int port) {
    connections.push_back(Connection(host, port, 1));
}

const std::vector<GearmanWorkerPtr>& GearmanWorkerBuilder::buildWorkers() {
    const EventLoopPoolPtr& pool = getChildPool();

    if (pool) {
        AsioServicePool::Iterator itr = pool->begin();

        for (; itr != pool->end(); ++itr) {
            buildWorker(*itr);
        }
    }

    return workers;
}

void GearmanWorkerBuilder::buildWorker(const EventLoopPtr& eventLoop) {
    if (!pipeline) {
        pipeline = getDefaultPipeline();
    }

    //to connect to remote at this point
    GearmanWorkerPtr worker =
        new GearmanWorker(eventLoop, pipeline, connections);

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

ChannelPipelinePtr GearmanWorkerBuilder::getDefaultPipeline() {
    ChannelPipelinePtr pipeline = ChannelPipelines::pipeline();

    pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 8, 4, 0, 4));
    pipeline->addLast("gearmanDecoder", new GearmanDecoder());
    pipeline->addLast("gearmanEncoder", new GearmanEncoder());
    pipeline->addLast("gearmanWorker", new GearmanWorkerHandler());
    return pipeline;
}

void GearmanWorkerBuilder::registerWorker(const std::string& functionName,
        const WorkerFunctor& worker) {
    if (!pipeline) {
        pipeline = getDefaultPipeline();
    }

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
