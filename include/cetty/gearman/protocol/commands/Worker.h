#if !defined(CETTY_GEARMAN_PROTOCOL_COMMANDS_WORKER_H)
#define CETTY_GEARMAN_PROTOCOL_COMMANDS_WORKER_H

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

#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/gearman/protocol/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {
namespace protocol {
namespace commands {

using namespace cetty::buffer;

// worker  request
GearmanMessagePtr candoMessage(const std::string& functionName);

GearmanMessagePtr candoTimeoutMessage(const std::string& functionName, int timeout);
GearmanMessagePtr cantdoMessage(const std::string& functionName);
GearmanMessagePtr resetAbilitiesMessage();
GearmanMessagePtr preSleepMessage();
GearmanMessagePtr grabJobMessage();
GearmanMessagePtr grabJobUniqMessage();
GearmanMessagePtr workStautsMessage(const std::string& jobHandle,
                                    int numerator,
                                    int denominator);
GearmanMessagePtr workCompleteMessage(const std::string& jobHandle,
                                      const ChannelBufferPtr& payload);
GearmanMessagePtr workFailMessage(const std::string& jobHandle);
GearmanMessagePtr workWarningMessage(const std::string& jobHandle,
                                     const ChannelBufferPtr& payload);
GearmanMessagePtr workExceptionMessage(const std::string& jobHandle,
                                       const ChannelBufferPtr& payload);
GearmanMessagePtr workDataMessage(const std::string& jobHandle,
                                  const ChannelBufferPtr& payload);

GearmanMessagePtr setClientIdMessage(const std::string& clientId);
GearmanMessagePtr allYoursMessage();

}
}
}
}


#endif //#if !defined(CETTY_GEARMAN_PROTOCOL_COMMANDS_WORKER_H)

// Local Variables:
// mode: c++
// End:
