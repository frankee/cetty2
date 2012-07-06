#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICEREQUESTMAPPER_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICEREQUESTMAPPER_H

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
#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/protobuf/service/http/map/ServiceMapperPtr.h>
#include <cetty/protobuf/service/http/map/HttpServiceTemplate.h>

namespace cetty {
namespace config {
class ConfigCenter;
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::config;
using namespace cetty::handler::codec::http;

class ServiceRequestMapper : public cetty::util::ReferenceCounter<ServiceRequestMapper, int> {
public:
    ServiceRequestMapper();
    ServiceRequestMapper(const std::string& conf);
    ServiceRequestMapper(const ConfigCenter& confCenter);

    int configure(const char* conf);
    int configure(const std::string& conf);
    int configure(const ConfigCenter& confCenter);
    int configureFromFile(const std::string& file);

    HttpServiceTemplate* match(const HttpMethod& method, const std::vector<std::string>& pathSegments);

private:
    std::vector<HttpServiceTemplate> serviceTemplates;
};

}
}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICEREQUESTMAPPER_H)

// Local Variables:
// mode: c++
// End:
