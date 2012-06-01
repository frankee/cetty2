#if !defined(CETTY_HANDLER_RPC_PROTOBUF_DEFAULTHTTPMESSAGEHANDLER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_DEFAULTHTTPMESSAGEHANDLER_H

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

class DefaultHttpMessageHandler : public ProtobufHttpMessageHandler {
public:
    static const std::string OUTPUT_TYPE_PARAM_NAME;

public:
    DefaultHttpMessageHandler();
    virtual ~DefaultHttpMessageHandler();

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

    void setServiceMatchPartten(const std::string& serviceName,
                                const std::string& matchKey,
                                int matchIndex);

    void setMethodMatchPartten(const std::string& serviceName,
                               const std::string& methodName,
                               const std::string& matchKey,
                               int matchIndex);

    void setMethodParamMatchPartten(const std::string& param, const std::string& matchKey, int matchIndex);
    void setMethodParamMatchPartten(const std::string& serviceName,
                                    const std::string& param,
                                    const std::string& matchKey,
                                    int matchIndex);
    void setMethodParamMatchPartten(const std::string& serviceName,
                                    const std::string& methodName,
                                    const std::string& param,
                                    const std::string& matchKey,
                                    int matchIndex);

protected:
    virtual int  getRpcInfo(const HttpRequestPtr& request, std::string* service, std::string* method);
    virtual int  decodeRequest(RequestContext& context);
    virtual void encodeResponse(RequestContext& context);

private:
    struct MatchPartten {
        int index;
        std::string key;
    };

    struct ServiceKey {
        std::string service;

        ServiceKey() {}
        ServiceKey(const std::string& service) : service(service) {}

        bool operator < (const ServiceKey& key) const {
            return service < key.service;
        }
    };

    struct MethodKey {
        std::string service;
        std::string method;

        MethodKey() {}
        MethodKey(const std::string& service, const std::string& method) : service(service), method(method) {}

        bool operator < (const MethodKey& key) const {
            if (service == key.service) {
                return method < key.method;
            }

            return service < key.service;
        }
    };

    struct MethodParamKey {
        std::string service;
        std::string method;
        std::string param;

        MethodParamKey() {}
        MethodParamKey(const std::string& service, const std::string& method, const std::string& param)
            : service(service), method(method), param(param) {}

        bool operator < (const MethodParamKey& key) const {
            if (service == key.service) {
                if (method == key.method) {
                    return param < key.param;
                }

                return method < key.method;
            }

            return service < key.service;
        }
    };

    enum OutPutType {
        OUTPUT_JSON,
        OUTPUT_XML,
        OUTPUT_PROTOBUF,
    };

    typedef std::map<ServiceKey, MatchPartten>     ServiceMatchMap;
    typedef std::map<MethodKey, MatchPartten>      MethodMatchMap;
    typedef std::map<MethodParamKey, MatchPartten> MethodParamMatchMap;

private:

    bool match(const MatchPartten& partten,
               const std::vector<std::string>& pathSegments,
               const NameValueCollection& queryParams);

private:
    ServiceMatchMap serviceMatchParttens;
    MethodMatchMap  methodMatchParttens;

    MethodParamMatchMap methodParamMatchParttens;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_DEFAULTHTTPMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:

