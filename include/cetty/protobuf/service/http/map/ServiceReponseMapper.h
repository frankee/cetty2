#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICEREPONSEMAPPER_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICEREPONSEMAPPER_H

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
#include <cetty/util/ReferenceCounter.h>
#include <cetty/handler/codec/http/HttpMessageFwd.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::handler::codec::http;

class ServiceReponseMapper : public cetty::util::ReferenceCounter<ServiceReponseMapper> {
public:
    struct MapKey {
        std::string service;
        std::string method;
    };

    struct MapValue {
        std::string content;
        std::map<std::string, std::string> headers;
    };

public:
    ServiceReponseMapper();

    const MapValue* match(const std::string& service, const std::string& method);

    static void setHttpHeaders(const MapValue& value, const HttpResponsePtr& response);
    static void setHttpContent(const MapValue& value, const HttpResponsePtr& response);

private:
    std::map<MapKey, MapValue> maps;
};

}
}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICEREPONSEMAPPER_H)

// Local Variables:
// mode: c++
// End:
