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
namespace proto {
class ServiceMessage;
}
}
}
}

namespace cetty {
namespace protobuf {
namespace service {

using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::proto;

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
    ProtobufServiceMessage(int type, boost::int64_t id);
    ProtobufServiceMessage(int type, boost::int64_t id, const MessagePtr& payload);
    ProtobufServiceMessage(int type, const std::string& service, const std::string& method);
    ProtobufServiceMessage(int type, const std::string& service, const std::string& method, const MessagePtr& payload);
    ~ProtobufServiceMessage();

    bool isRequest() const;
    bool isResponse() const;
    bool isError() const;

    void setType(int type);

    boost::int64_t getId() const;
    void setId(boost::int64_t id);

    const std::string& getService() const;
    std::string* mutableService();

    const std::string& getMethod() const;
    std::string* mutableMethod();

    const ServiceMessage& getServiceMessage() const;
    ServiceMessage* mutableServiceMessage() {
        return message;
    }

    const MessagePtr& getPayload() const {
        return payload;
    }

    ConstMessagePtr getConstPayload() const {
        return payload.get();
    }

    void setPayload(const MessagePtr& message) {
        this->payload = message;
    }

    const MessagePtr& getResponse() const;
    const MessagePtr& getRequest() const;

    int getMessageSize() const;

private:
    bool checkType(int type);

private:
    ProtobufServiceMessage(const ProtobufServiceMessage&);
    ProtobufServiceMessage& operator=(const ProtobufServiceMessage&);

private:
    ServiceMessage* message;
    MessagePtr payload;
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEMESSAGE_H)

// Local Variables:
// mode: c++
// End:
