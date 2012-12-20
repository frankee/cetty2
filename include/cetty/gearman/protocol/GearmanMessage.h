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
