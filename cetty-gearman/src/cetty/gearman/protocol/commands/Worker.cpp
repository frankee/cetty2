
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

GearmanMessagePtr GearmanMessage::createCandoMessage(const std::string& functionName) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::CAN_DO,functionName));
}

GearmanMessagePtr GearmanMessage::createCandoTimeoutMessage(const std::string& functionName, int timeout) {
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::CAN_DO_TIMEOUT,functionName,StringUtil::printf("%d",timeout)));
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
    return GearmanMessagePtr(new GearmanMessage(GearmanMessage::WORK_STATUS,jobHandle,StringUtil::printf("%d",numerator),StringUtil::printf("%d",numerator)));
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

}
}
}
}
