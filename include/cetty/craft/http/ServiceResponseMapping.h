#if !defined(CETTY_CRAFT_HTTP_SERVICERESPONSEMAPPER_H)
#define CETTY_CRAFT_HTTP_SERVICERESPONSEMAPPER_H

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

#include <map>
#include <boost/noncopyable.hpp>
#include <cetty/handler/codec/http/HttpPackage.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace craft {
namespace http {

using namespace google::protobuf;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

class ServiceResponseMapping : private boost::noncopyable {
public:
    typedef std::map<std::string, NameValueCollection> Headers;

public:
    static ServiceResponseMapping& instance();

public:
    HttpResponsePtr toHttpResponse(const HttpRequestPtr& req,
                                   const ProtobufServiceMessagePtr& message);

private:
    ServiceResponseMapping();

    void setHttpHeaders(const std::string& method,
                        const HttpResponsePtr& response);

    void setHttpContent(const Message& message,
                        const std::string& producer,
                        const HttpResponsePtr& response);

    void onServiceRegistered(const ProtobufServicePtr& service);

    void parseHeaders(const std::string& headerStr, NameValueCollection* headers);

private:
    Headers headers_;

private:
    static ServiceResponseMapping* mapping_;
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_HTTP_SERVICERESPONSEMAPPER_H)

// Local Variables:
// mode: c++
// End:
