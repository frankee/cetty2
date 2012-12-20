
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

namespace cetty {
namespace gearman {
namespace protocol {
namespace commands {

    enum GearmanMessageType {
        CAN_DO          =  1,
        CANT_DO         =  2,
        RESET_ABILITIES =  3,
        PRE_SLEEP       =  4,
        NOOP            =  6,
        SUBMIT_JOB      =  7,
        JOB_CREATED     =  8,
        GRAB_JOB        =  9,
        NO_JOB          = 10,
        JOB_ASSIGN      = 11,
        WORK_STATUS     = 12,
        WORK_COMPLETE   = 13,
        WORK_FAIL       = 14,
        GET_STATUS      = 15,
        ECHO_REQ        = 16,
        ECHO_RES        = 17,
        SUBMIT_JOB_BG   = 18,
        ERROR           = 19,
        STATUS_RES      = 20,
        SUBMIT_JOB_HIGH = 21,
        SET_CLIENT_ID   = 22,
        CAN_DO_TIMEOUT  = 23,
        ALL_YOURS       = 24,
        WORK_EXCEPTION  = 25,

        OPTION_REQ      = 26,
        OPTION_RES      = 27,
        WORK_DATA       = 28,

        WORK_WARNING    = 29,

        GRAB_JOB_UNIQ   = 30,
        JOB_ASSIGN_UNIQ = 31,
        SUBMIT_JOB_HIGH_BG = 32,
        SUBMIT_JOB_LOW     = 33,
        SUBMIT_JOB_LOW_BG  = 34,
        SUBMIT_JOB_SCHED   = 35,
        SUBMIT_JOB_EPOCH   = 36
    };

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
}
}
