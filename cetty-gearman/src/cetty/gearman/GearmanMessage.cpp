
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

#include <cetty/gearman/GearmanMessage.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace gearman {

const std::string GearmanMessage::RESPONSE_MAGIC("\0RES", 4);
const std::string GearmanMessage::REQUEST_MAGIC("\0REQ", 4);

GearmanMessage::GearmanMessage(int type) {
    this->type = type;
}
GearmanMessage::GearmanMessage(int type, const ChannelBufferPtr& payload) {
    this->type = type;
    this->data = payload;
}
GearmanMessage::GearmanMessage(int type, const std::string& param) {
    this->type = type;
    this->parameters.push_back(param);
}
GearmanMessage::GearmanMessage(int type, const std::string& param, const ChannelBufferPtr& payload) {
    this->type = type;

    if (param.size()>0) {
        this->parameters.push_back(param);
    }

    this->data = payload;
}

GearmanMessage::GearmanMessage(int type, const std::string& param1, const std::string& param2) {
    this->type = type;
    this->parameters.push_back(param1);
    this->parameters.push_back(param2);
}

GearmanMessage::GearmanMessage(int type, const std::string& param1, const std::string& param2, const std::string& param3) {
    this->type = type;
    this->parameters.push_back(param1);
    this->parameters.push_back(param2);
    this->parameters.push_back(param3);
}
GearmanMessage::GearmanMessage(int type, const std::string& param1, const std::string& param2, const ChannelBufferPtr& payload) {
    this->type = type;
    this->parameters.push_back(param1);
    this->parameters.push_back(param2);
    this->data = payload;
}

GearmanMessagePtr GearmanMessage::createEchoReqMessage(const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::ECHO_REQ,payload));
}
//worker
GearmanMessagePtr GearmanMessage::createCandoMessage(const std::string& functionName) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::CAN_DO,functionName));
}

GearmanMessagePtr GearmanMessage::createCandoTimeoutMessage(const std::string& functionName, int timeout) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::CAN_DO_TIMEOUT,functionName,StringUtil::strprintf("%d",timeout)));
}

GearmanMessagePtr GearmanMessage::createCantdoMessage(const std::string& functionName) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::CANT_DO,functionName));
}
GearmanMessagePtr GearmanMessage::createResetAbilitiesMessage() {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::RESET_ABILITIES));
}
GearmanMessagePtr GearmanMessage::createPreSleepMessage() {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::PRE_SLEEP));
}
GearmanMessagePtr GearmanMessage::createGrabJobMessage() {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::GRAB_JOB));
}
GearmanMessagePtr GearmanMessage::createGrabJobUniqMessage() {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::GRAB_JOB_UNIQ));
}

GearmanMessagePtr GearmanMessage::createWorkStautsMessage(const std::string& jobHandle, int numerator, int denominator) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::WORK_STATUS,jobHandle,StringUtil::strprintf("%d",numerator),StringUtil::strprintf("%d",numerator)));
}
GearmanMessagePtr GearmanMessage::createWorkCompleteMessage(const std::string& jobHandle, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::WORK_COMPLETE,jobHandle,payload));
}
GearmanMessagePtr GearmanMessage::createWorkFailMessage(const std::string& jobHandle) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::WORK_FAIL,jobHandle));
}
GearmanMessagePtr GearmanMessage::createWorkWarningMessage(const std::string& jobHandle, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::WORK_WARNING,jobHandle,payload));
}
GearmanMessagePtr GearmanMessage::createWorkExceptionMessage(const std::string& jobHandle, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::WORK_EXCEPTION,jobHandle,payload));
}
GearmanMessagePtr GearmanMessage::createWorkDataMessage(const std::string& jobHandle, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::WORK_DATA,jobHandle,payload));
}

GearmanMessagePtr GearmanMessage::createSetClientIdMessage(const std::string& clientId) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::SET_CLIENT_ID,clientId));
}
GearmanMessagePtr GearmanMessage::createAllYoursMessage() {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::ALL_YOURS));
}


// client
GearmanMessagePtr GearmanMessage::createsubmitJobMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::SUBMIT_JOB,functionName,uniqueId,payload));
}
GearmanMessagePtr GearmanMessage::createsubmitJobHighMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::SUBMIT_JOB_HIGH,functionName,uniqueId,payload));
}
GearmanMessagePtr GearmanMessage::createsubmitJobLowMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::SUBMIT_JOB_LOW,functionName,uniqueId,payload));
}

GearmanMessagePtr GearmanMessage::createsubmitJobBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::SUBMIT_JOB_BG,functionName,uniqueId,payload));
}
GearmanMessagePtr GearmanMessage::createsubmitJobHighBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::SUBMIT_JOB_HIGH_BG,functionName,uniqueId,payload));
}
GearmanMessagePtr GearmanMessage::createsubmitJobLowBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::SUBMIT_JOB_LOW_BG,functionName,uniqueId,payload));
}

GearmanMessagePtr GearmanMessage::createGetStatusMessage(const std::string& jobHandle) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::GET_STATUS,jobHandle));
}
GearmanMessagePtr GearmanMessage::createOptionReqMessage(const std::string& option) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::OPTION_REQ));
}

}
}
