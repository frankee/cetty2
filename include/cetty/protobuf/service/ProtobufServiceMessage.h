#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEMESSAGE_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEMESSAGE_H

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

#include <cetty/util/ReferenceCounter.h>
#include <cetty/protobuf/service/ProtobufServiceFuture.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace cetty {
namespace protobuf {
namespace service {

using namespace cetty::protobuf::service;

class ServiceMessage;

class ProtobufServiceMessage : public cetty::util::ReferenceCounter<ProtobufServiceMessage> {
public:
    enum Type {
        T_REQUEST  = 1,
        T_RESPONSE = 2,
        T_ERROR    = 3
    };

public:
    ProtobufServiceMessage();
    ProtobufServiceMessage(int type);
    ProtobufServiceMessage(int type, int64_t id);
    ProtobufServiceMessage(int type, int64_t id, const MessagePtr& payload);
    ProtobufServiceMessage(int type, const std::string& service, const std::string& method);
    ProtobufServiceMessage(int type, const std::string& service, const std::string& method, const MessagePtr& payload);
    ProtobufServiceMessage(int type, int64_t id, const std::string& service, const std::string& method, const MessagePtr& payload);
    ~ProtobufServiceMessage();

    bool isRequest() const;
    bool isResponse() const;
    bool isError() const;
    
    int type() const;
    void setType(int type);

    int64_t id() const;
    void setId(int64_t id);

    const std::string& service() const;
    std::string* mutableService();

    const std::string& method() const;
    std::string* mutableMethod();

    void methodFullName(std::string* name) const;

    const ServiceMessage& serviceMessage() const;
    ServiceMessage* mutableServiceMessage() {
        return message_;
    }

    const MessagePtr& payload() const {
        return payload_;
    }

    void setPayload(const MessagePtr& message) {
        this->payload_ = message;
    }

    const MessagePtr& response() const;
    const MessagePtr& request() const;

    int messageSize() const;

private:
    bool checkType(int type);

private:
    ProtobufServiceMessage(const ProtobufServiceMessage&);
    ProtobufServiceMessage& operator=(const ProtobufServiceMessage&);

private:
    ServiceMessage* message_;
    MessagePtr payload_;
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEMESSAGE_H)

// Local Variables:
// mode: c++
// End:
