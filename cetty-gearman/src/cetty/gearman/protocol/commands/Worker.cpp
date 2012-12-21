
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

#include <cetty/gearman/protocol/commands/Worker.h>
#include <cetty/gearman/protocol/GearmanMessage.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace gearman {
namespace protocol {
namespace commands {

using namespace cetty::util;

GearmanMessagePtr candoMessage(const std::string& functionName) {
    return new GearmanMessage(GearmanMessage::CAN_DO,functionName);
}

GearmanMessagePtr candoTimeoutMessage(const std::string& functionName, int timeout) {
    return new GearmanMessage(GearmanMessage::CAN_DO_TIMEOUT,
                              functionName,
                              StringUtil::numtostr(timeout));
}

GearmanMessagePtr cantdoMessage(const std::string& functionName) {
    return new GearmanMessage(GearmanMessage::CANT_DO, functionName);
}

GearmanMessagePtr resetAbilitiesMessage() {
    return new GearmanMessage(GearmanMessage::RESET_ABILITIES);
}

GearmanMessagePtr preSleepMessage() {
    return new GearmanMessage(GearmanMessage::PRE_SLEEP);
}

GearmanMessagePtr grabJobMessage() {
    return new GearmanMessage(GearmanMessage::GRAB_JOB);
}

GearmanMessagePtr grabJobUniqMessage() {
    return new GearmanMessage(GearmanMessage::GRAB_JOB_UNIQ);
}

GearmanMessagePtr workStautsMessage(const std::string& jobHandle,
                                    int numerator,
                                    int denominator) {
    return new GearmanMessage(GearmanMessage::WORK_STATUS,
                              jobHandle,
                              StringUtil::numtostr(numerator),
                              StringUtil::numtostr(denominator));
}

GearmanMessagePtr workCompleteMessage(const std::string& jobHandle,
                                      const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::WORK_COMPLETE,
                              jobHandle,
                              payload);
}

GearmanMessagePtr workFailMessage(const std::string& jobHandle) {
    return new GearmanMessage(GearmanMessage::WORK_FAIL, jobHandle);
}

GearmanMessagePtr workWarningMessage(const std::string& jobHandle,
                                     const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::WORK_WARNING,
                              jobHandle,
                              payload);
}

GearmanMessagePtr workExceptionMessage(const std::string& jobHandle,
                                       const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::WORK_EXCEPTION,
                              jobHandle,
                              payload);
}

GearmanMessagePtr workDataMessage(const std::string& jobHandle,
                                  const ChannelBufferPtr& payload) {
    return new GearmanMessage(GearmanMessage::WORK_DATA,
                              jobHandle,
                              payload);
}

GearmanMessagePtr setClientIdMessage(const std::string& clientId) {
    return new GearmanMessage(GearmanMessage::SET_CLIENT_ID, clientId);
}

GearmanMessagePtr allYoursMessage() {
    return new GearmanMessage(GearmanMessage::ALL_YOURS);
}

}
}
}
}
