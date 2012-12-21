#if !defined(CETTY_GEARMAN_PROTOCOL_GEARMANMESSAGE_H)
#define CETTY_GEARMAN_PROTOCOL_GEARMANMESSAGE_H

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
#include <cetty/gearman/protocol/GearmanMessagePtr.h>

#if defined(WIN32) && defined(ERROR)
#undef ERROR
#endif

namespace cetty {
namespace gearman {
namespace protocol {

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

public:
    static const std::string REQUEST_MAGIC;
    static const std::string RESPONSE_MAGIC;

public:
    GearmanMessage() {}
    GearmanMessage(int type);
    GearmanMessage(int type, const ChannelBufferPtr& payload);

    GearmanMessage(int type, const std::string& param);
    GearmanMessage(int type,
        const std::string& param,
        const ChannelBufferPtr& payload);

    GearmanMessage(int type,
        const std::string& param1,
        const std::string& param2);
    GearmanMessage(int type,
        const std::string& param1,
        const std::string& param2,
        const std::string& param3);
    GearmanMessage(int type,
        const std::string& param1,
        const std::string& param2,
        const ChannelBufferPtr& payload);

    ~GearmanMessage() {}

    bool hasData() const { return data_ && data_->readable(); }

    const std::vector<std::string>& parameters() const { return parameters_; }
    const std::string& getParameter(int index) { return parameters_.at(index); }

    const ChannelBufferPtr& data()const { return data_; }
    void setData(const ChannelBufferPtr& data) {
        this->data_ = data;
    }

    int type() const { return type_; }
    void setType(int type) { this->type_ = type; }

    std::string* addParameter() {
        parameters_.resize(parameters_.size()+1);
        return &parameters_.back();
    }

private:
    GearmanMessage(const GearmanMessage&);
    GearmanMessage& operator=(const GearmanMessage&);

private:
    int type_;
    ChannelBufferPtr data_;
    std::vector<std::string> parameters_;
};

}
}
}

#endif //#if !defined(CETTY_GEARMAN_PROTOCOL_GEARMANMESSAGE_H)
