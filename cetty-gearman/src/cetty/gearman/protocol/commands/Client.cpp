
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

#include <cetty/gearman/protocol/commands/Client.h>
#include <cetty/gearman/protocol/GearmanMessage.h>

namespace cetty {
namespace gearman {
namespace protocol {
namespace commands {

// client
GearmanMessagePtr submitJobMessage(const std::string& functionName,
                                   const std::string& uniqueId,
                                   const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::SUBMIT_JOB,
                              functionName,
                              uniqueId,
                              payload);
}

GearmanMessagePtr submitJobHighMessage(const std::string& functionName,
                                       const std::string& uniqueId,
                                       const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::SUBMIT_JOB_HIGH,
                              functionName,
                              uniqueId,
                              payload);
}

GearmanMessagePtr submitJobLowMessage(const std::string& functionName,
                                      const std::string& uniqueId,
                                      const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::SUBMIT_JOB_LOW,
                              functionName,
                              uniqueId,
                              payload);
}

GearmanMessagePtr submitJobBGMessage(const std::string& functionName,
                                     const std::string& uniqueId,
                                     const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::SUBMIT_JOB_BG,
                              functionName,
                              uniqueId,
                              payload);
}

GearmanMessagePtr submitJobHighBGMessage(const std::string& functionName,
        const std::string& uniqueId,
        const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::SUBMIT_JOB_HIGH_BG,
                              functionName,
                              uniqueId,
                              payload);
}

GearmanMessagePtr submitJobLowBGMessage(const std::string& functionName,
                                        const std::string& uniqueId,
                                        const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::SUBMIT_JOB_LOW_BG,
                              functionName,
                              uniqueId,
                              payload);
}

GearmanMessagePtr getStatusMessage(const std::string& jobHandle) {
    return new GearmanMessage(GearmanMessage::GET_STATUS,
                              jobHandle);
}

GearmanMessagePtr optionReqMessage(const std::string& option) {
    return new GearmanMessage(GearmanMessage::OPTION_REQ);
}

}
}
}
}
