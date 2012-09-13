#if !defined(CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPER_H)
#define CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPER_H

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
#include <cetty/craft/http/ServiceMapperPtr.h>
#include <cetty/craft/http/HttpRequestTemplate.h>
#include <cetty/craft/http/ServiceRequestMapperConfig.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::config;
using namespace cetty::handler::codec::http;

class ServiceRequestMapper
    : public cetty::util::ReferenceCounter<ServiceRequestMapper, int> {
public:
    ServiceRequestMapper();
    ServiceRequestMapper(const std::string& conf);
    virtual ~ServiceRequestMapper();

    bool configure(const char* conf);
    bool configure(const std::string& conf);
    bool configure(const ServiceRequestMapperConfig& conf);
    bool configureFromFile(const std::string& file);

    HttpRequestTemplate* match(const HttpMethod& method, const std::vector<std::string>& pathSegments);

private:
    bool init();
    void deinit();

private:
    ServiceRequestMapperConfig config;

    std::vector<HttpRequestTemplate*> serviceTemplates;
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPER_H)

// Local Variables:
// mode: c++
// End: