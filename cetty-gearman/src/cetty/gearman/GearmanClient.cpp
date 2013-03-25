
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

#include <cetty/gearman/GearmanClient.h>

#include <cetty/service/ClientService.h>
#include <cetty/gearman/protocol/GearmanMessage.h>
#include <cetty/gearman/protocol/commands/Client.h>
#include <cetty/gearman/protocol/commands/Common.h>

namespace cetty {
namespace gearman {

using namespace cetty::service;
using namespace cetty::gearman::protocol;

GearmanServiceFuturePtr GearmanClient::submitJob(const std::string& functionName,
        const ChannelBufferPtr& payload) {
    GearmanMessagePtr command = commands::submitJobMessage(functionName, "", payload);
    GearmanServiceFuturePtr future = new GearmanServiceFuture;
    cetty::service::callMethod(channel_, command, future);
    return future;
}

GearmanServiceFuturePtr GearmanClient::submitJob(const std::string& functionName,
        const ChannelBufferPtr& payload,
        const std::string& uniqueId,
        Priority priority,
        bool background) {
    return GearmanServiceFuturePtr();
}

GearmanServiceFuturePtr GearmanClient::submitJob(const std::string& functionName,
        const ChannelBufferPtr& payload,
        const std::string& uniqueId,
        const boost::posix_time::time_duration& later) {
    return GearmanServiceFuturePtr();
}

GearmanServiceFuturePtr GearmanClient::submitJob(const std::string& functionName,
        const ChannelBufferPtr& payload,
        const std::string& uniqueId,
        const boost::posix_time::ptime& time) {
    return GearmanServiceFuturePtr();
}

GearmanServiceFuturePtr GearmanClient::getStatus(const std::string& jobId) {
    return GearmanServiceFuturePtr();
}

GearmanServiceFuturePtr GearmanClient::setOption(const std::string& key,
        const std::string& value) {
    return GearmanServiceFuturePtr();
}

}
}
