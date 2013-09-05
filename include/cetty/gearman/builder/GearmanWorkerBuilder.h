#if !defined(CETTY_GEARMAN_BUILDER_GEARMANWORKERBUILDER_H)
#define CETTY_GEARMAN_BUILDER_GEARMANWORKERBUILDER_H
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

#include <vector>
#include <boost/function.hpp>

#include <cetty/service/Connection.h>
#include <cetty/service/builder/ClientBuilder.h>
#include <cetty/gearman/GearmanWorker.h>
#include <cetty/gearman/protocol/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {
namespace builder {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::service::builder;
using namespace cetty::gearman::protocol;

class GearmanWorkerBuilder : private boost::noncopyable {
public:
    typedef ChannelPipeline::Initializer PipelineInitializer;
    typedef boost::function<GearmanMessagePtr(const GearmanMessagePtr&)> WorkFunctor;

public:
    GearmanWorkerBuilder();
    GearmanWorkerBuilder(int threadCnt);
    GearmanWorkerBuilder(const EventLoopPtr& eventLoop);
    GearmanWorkerBuilder(const EventLoopPoolPtr& eventLoopPool);

    GearmanWorkerBuilder& addConnection(const std::string& host, int port);

    GearmanWorkerBuilder& registerWorker(const std::string& functionName,
                                         const WorkFunctor& worker);

    void setAdditionalInitializer(const PipelineInitializer& initializer);

    GearmanWorkerBuilder& buildWorkers();

    void waitingForExit();

private:
    void buildWorker(const EventLoopPtr& eventLoop);
    bool initializeChannel(ChannelPipeline& pipeline);

private:
    typedef std::map<std::string, WorkFunctor> WorkFunctors;

private:
    EventLoopPtr loop_;
    EventLoopPoolPtr pool_;
    WorkFunctors workFunctors_;
    PipelineInitializer additionalInitializer_;

    std::vector<Connection> connections_;
    std::vector<GearmanWorkerPtr> workers_;
};

inline
GearmanWorkerBuilder& GearmanWorkerBuilder::addConnection(
    const std::string& host,
    int port) {
    connections_.push_back(Connection(host, port, 1));
    return *this;
}

inline
void GearmanWorkerBuilder::setAdditionalInitializer(
    const PipelineInitializer& initializer) {
    additionalInitializer_ = initializer;
}

}
}
}
#endif //#if !defined(CETTY_GEARMAN_BUILDER_GEARMANWORKERBUILDER_H)

// Local Variables:
// mode: c++
// End:
