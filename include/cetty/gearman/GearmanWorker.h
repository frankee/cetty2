#if !defined(CETTY_GEARMAN_GEARMANWORKER_H)
#define CETTY_GEARMAN_GEARMANWORKER_H

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

#include <cetty/util/ReferenceCounter.h>
#include <cetty/service/Connection.h>
#include <cetty/service/pool/WatermarkConnectionPool.h>
#include <cetty/gearman/GearmanWorkerPtr.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::service::pool;

class GearmanWorker : public cetty::util::ReferenceCounter<GearmanWorker, int> {
public:
    typedef ChannelPipeline::Initializer PipelineInitializer;

public:
    GearmanWorker(const EventLoopPtr& eventLoop,
                  const PipelineInitializer& initializer,
                  const Connections& connections);

    virtual ~GearmanWorker();

    void setCando(const std::string& cando);
    void setCando(const std::string& cando, int64_t timeout);

    void setCantdo(const std::string& cantdo);
    void resetAbilities();
    
    void grabJob();
    void grabUniqueJob();

private:
    WatermarkConnectionPool connectionPool_;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANWORKER_H)

// Local Variables:
// mode: c++
// End:
