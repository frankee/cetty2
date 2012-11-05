#if !defined(CETTY_GEARMAN_GEARMANCLIENT_H)
#define CETTY_GEARMAN_GEARMANCLIENT_H

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
#include <cetty/service/ClientService.h>
#include <cetty/gearman/GearmanServiceFuture.h>
#include <cetty/gearman/GearmanClientRequestHandler.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::service;

class GearmanClient : public cetty::util::ReferenceCounter<GearmanClient, int> {
public:
    enum Priority {
        NORMAL,
        HIGH,
        LOW
    };

public:
    GearmanClient(const ClientServicePtr& service) {}
    virtual ~GearmanClient() {}

    GearmanServiceFuturePtr submitJob(const std::string& functionName,
                                      const std::string& uniqueId,
                                      const ChannelBufferPtr& payload,
                                      Priority priority,
                                      bool background);

    GearmanServiceFuturePtr submitJob(const std::string& functionName,
                                      const std::string& uniqueId,
                                      const ChannelBufferPtr& payload,
                                      const boost::posix_time::time_duration& later);

    GearmanServiceFuturePtr submitJob(const std::string& functionName,
                                      const std::string& uniqueId,
                                      const ChannelBufferPtr& payload,
                                      const boost::posix_time::ptime& time);

    GearmanServiceFuturePtr getStatus(const std::string& jobId);

    GearmanServiceFuturePtr setOption(const std::string& key,
                                      const std::string& value);

private:
    ClientServicePtr service;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANCLIENT_H)

// Local Variables:
// mode: c++
// End:
