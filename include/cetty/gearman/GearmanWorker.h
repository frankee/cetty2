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

#include <boost/intrusive_ptr.hpp>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/service/pool/WatermarkConnectionPool.h>

namespace cetty {
namespace gearman {

class GearmanWorker;
typedef boost::intrusive_ptr<GearmanWorker> GearmanWorkerPtr;

class GearmanWorker : public cetty::util::ReferenceCounter<GearmanWorker, int> {
public:
    GearmanWorker();
    GearmanWorker(const AsioServicePtr& ioService);

private:
    void init() {
        ChannelPipelinePtr pipeline = Channels::pipeline();

        pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 0, 4, 0, 4));

        pipeline->addLast("gearmanDecoder", new GearmanDecoder());
        pipeline->addLast("gearmanEncoder", new GearmanEncoder());
        pipeline->addLast("gearmanWorker", new GearmanWorkerHandler());

        connectionPool.getBootstrap().setPipeline(pipeline);
        connectionPool.getBootstrap().setFactory(new AsioClientSocketChannelFactory(ioService));
    }

private:
    AsioServicePtr ioService;
    WatermarkConnectionPool connectionPool;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANWORKER_H)

// Local Variables:
// mode: c++
// End:
