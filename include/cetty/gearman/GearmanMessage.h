#if !defined(CETTY_GEARMAN_GEARMANMESSAGE_H)
#define CETTY_GEARMAN_GEARMANMESSAGE_H

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
#include <vector>
#include <string>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/gearman/GearmanMessagePtr.h>

#if defined(WIN32) && defined(ERROR)
#undef ERROR
#endif

namespace cetty {
namespace gearman {

using namespace cetty::buffer;

class GearmanMessage : public cetty::util::ReferenceCounter<GearmanMessage, int> {
public:
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

    static const std::string REQUEST_MAGIC;
    static const std::string RESPONSE_MAGIC;

public:
    static GearmanMessagePtr createEchoReqMessage(const ChannelBufferPtr& payload);
    // worker  request
    static GearmanMessagePtr createCandoMessage(const std::string& functionName);
    static GearmanMessagePtr createCandoTimeoutMessage(const std::string& functionName, int timeout);
    static GearmanMessagePtr createCantdoMessage(const std::string& functionName);
    static GearmanMessagePtr createResetAbilitiesMessage();
    static GearmanMessagePtr createPreSleepMessage();
    static GearmanMessagePtr createGrabJobMessage();
    static GearmanMessagePtr createGrabJobUniqMessage();
    static GearmanMessagePtr createWorkStautsMessage(const std::string& jobHandle, int numerator, int denominator);
    static GearmanMessagePtr createWorkCompleteMessage(const std::string& jobHandle, const ChannelBufferPtr& payload);
    static GearmanMessagePtr createWorkFailMessage(const std::string& jobHandle);
    static GearmanMessagePtr createWorkWarningMessage(const std::string& jobHandle, const ChannelBufferPtr& payload);
    static GearmanMessagePtr createWorkExceptionMessage(const std::string& jobHandle, const ChannelBufferPtr& payload);
    static GearmanMessagePtr createWorkDataMessage(const std::string& jobHandle, const ChannelBufferPtr& payload);

    static GearmanMessagePtr createSetClientIdMessage(const std::string& clientId);
    static GearmanMessagePtr createAllYoursMessage();

    // client
    static GearmanMessagePtr createsubmitJobMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
    static GearmanMessagePtr createsubmitJobHighMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
    static GearmanMessagePtr createsubmitJobLowMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);

    //client submit-bg
    static GearmanMessagePtr createsubmitJobBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
    static GearmanMessagePtr createsubmitJobHighBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
    static GearmanMessagePtr createsubmitJobLowBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);

    static GearmanMessagePtr createGetStatusMessage(const std::string& jobHandle);
    static GearmanMessagePtr createOptionReqMessage(const std::string& option);

public:
    GearmanMessage() {}
    GearmanMessage(int type);
    GearmanMessage(int type, const ChannelBufferPtr& payload);
    GearmanMessage(int type, const std::string& param);
    GearmanMessage(int type, const std::string& param, const ChannelBufferPtr& payload);

    GearmanMessage(int type, const std::string& param1, const std::string& param2);
    GearmanMessage(int type, const std::string& param1, const std::string& param2, const std::string& param3);
    GearmanMessage(int type, const std::string& param1, const std::string& param2, const ChannelBufferPtr& payload);

    ~GearmanMessage() {}

    int getType() const { return type; }
    const std::vector<std::string>& getParameters() const { return parameters; }
    const std::string& getParamater(int index) { return parameters.at(index); }
    const ChannelBufferPtr& getData() const { return data; }

    bool hasData() const { return data && data->readable(); }

    void setType(int type) { this->type = type; }
    std::string* addParameter() {
        parameters.resize(parameters.size()+1);
        return &parameters.back();
    }

    void setData(const ChannelBufferPtr& data) {
        this->data = data;
    }

private:
    GearmanMessage(const GearmanMessage&);
    GearmanMessage& operator=(const GearmanMessage&);

private:
    int type;
    std::vector<std::string> parameters;
    ChannelBufferPtr data;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANMESSAGE_H)
