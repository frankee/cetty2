#if !defined(CETTY_HANDLER_RPC_PROTOBUF_ABSTRACTHTTPMESSAGEHANDLER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_ABSTRACTHTTPMESSAGEHANDLER_H

/**
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/handler/rpc/protobuf/ProtobufHttpMessageHandler.h>

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

using namespace cetty::channel;
using namespace cetty::handler::codec::http;

class AbstractHttpMessageHandler : public ProtobufHttpMessageHandler {
public:
    AbstractHttpMessageHandler() {}
    virtual ~AbstractHttpMessageHandler() {}

    void addServiceMatchPartten(const std::string& serviceName,
                                const std::string& matchKey,
                                int matchIndex);

    void addMethodMatchPartten(const std::string& serviceName,
                               const std::string& methodName,
                               const std::string& matchKey,
                               int matchIndex);

protected:
    virtual int  getRpcInfo(const HttpRequestPtr& request, std::string* service, std::string* method);
    virtual int  decodeRequest(RequestContext& context);
    virtual void encodeResponse(RequestContext& context);

    virtual HttpRpcType getRpcType() = 0;

private:
    struct MatchPartten {
        int index;
        std::string key;
        std::string value;

        MatchPartten() : index(0) {}
        MatchPartten(const std::string& value, const std::string& key, int index)
            : value(value), key(key), index(index) {}

        bool isValid() const { return !(key.empty() || value.empty() || index < 0); }
    };

    struct ServiceMatchPartten {
        MatchPartten service;
        std::vector<MatchPartten> methods;

        ServiceMatchPartten(const std::string& service, const std::string& key, int index)
            : service(service, key, index) {}

        void appendMethod(const std::string& method, const std::string& key, int index) {
            methods.push_back(MatchPartten(method, key, index));
        }
    };

private:
    bool match(const MatchPartten& partten,
               const std::vector<std::string>& pathSegments);

    ServiceMatchPartten* findServicePartten(const std::string& name) {
        std::vector<ServiceMatchPartten>::iterator itr;

        for (itr = serviceParttens.begin(); itr != serviceParttens.end(); ++itr) {
            if (name.compare(itr->service.value) == 0) {
                return &(*itr);
            }
        }

        return NULL;
    }

private:
    std::vector<ServiceMatchPartten> serviceParttens;

};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_ABSTRACTHTTPMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:

