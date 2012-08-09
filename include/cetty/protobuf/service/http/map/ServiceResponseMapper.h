#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICERESPONSEMAPPER_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICERESPONSEMAPPER_H

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
#include <cetty/handler/codec/http/HttpMessagePtr.h>
#include <cetty/protobuf/service/http/map/ServiceMapperPtr.h>
#include <cetty/protobuf/service/http/map/ServiceResponseMapperConfig.h>

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

class ServiceResponseMapper
    : public cetty::util::ReferenceCounter<ServiceResponseMapper, int> {
public:
    typedef ServiceResponseMapperConfig::Template ResponseTemplate;

public:
    ServiceResponseMapper();
    ServiceResponseMapper(const std::string& conf);

    bool configure(const std::string& conf);
    bool configureFromFile(const std::string& file);

    const ResponseTemplate* match(const std::string& service, const std::string& method) const;

    static void setHttpHeaders(const ResponseTemplate& templ, const HttpResponsePtr& response);

private:
    bool init();

private:
    typedef std::map<std::string, const ResponseTemplate*> ResponseTemplateMap;

private:
    ServiceResponseMapperConfig config;
    ResponseTemplateMap maps;
};

}
}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICERESPONSEMAPPER_H)

// Local Variables:
// mode: c++
// End:
