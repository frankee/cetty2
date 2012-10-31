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

#include <cetty/channel/ChannelPipelinePtr.h>
#include <cetty/service/Connection.h>
#include <cetty/service/builder/ServerBuilder.h>
#include <cetty/gearman/GearmanWorkerPtr.h>
#include <cetty/gearman/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {
namespace builder {

using namespace cetty::service;
using namespace cetty::service::builder;
using namespace cetty::gearman;

class GearmanWorkerBuilder : public cetty::service::builder::ServerBuilder {
public:
    typedef boost::function1<GearmanMessagePtr, const GearmanMessagePtr&> WorkerFunctor;

public:
    GearmanWorkerBuilder();
    GearmanWorkerBuilder(int threadCnt);
    virtual ~GearmanWorkerBuilder();

    void addConnection(const std::string& host, int port);

    void registerWorker(const std::string& functionName, const WorkerFunctor& worker);

    void setWorkerPipeline(const ChannelPipelinePtr& pipeline);
    const ChannelPipelinePtr& getWorkerPipeline();

    const std::vector<GearmanWorkerPtr>& buildWorkers();

protected:
    virtual ChannelPipelinePtr getDefaultPipeline();

private:
    void buildWorker(const EventLoopPtr& eventLoop);

private:
    std::vector<Connection> connections;
    std::vector<GearmanWorkerPtr> workers;

    ChannelPipelinePtr pipeline;
};

}
}
}
#endif //#if !defined(CETTY_GEARMAN_BUILDER_GEARMANWORKERBUILDER_H)

// Local Variables:
// mode: c++
// End:
