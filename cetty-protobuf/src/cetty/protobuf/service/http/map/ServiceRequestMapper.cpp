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

#include <cetty/protobuf/service/http/map/ServiceRequestMapper.h>

#include <cetty/config/ConfigCenter.h>


namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

//ServiceResponseMapperConfig repConfig;
//ServiceRequestMapperConfig reqConfig;
//config.configure(&reqConfig);
//config.configure(&repConfig);

ServiceRequestMapper::ServiceRequestMapper() {
}

ServiceRequestMapper::ServiceRequestMapper(const std::string& conf) {
}

ServiceRequestMapper::ServiceRequestMapper(const ConfigCenter& confCenter) {
}

int ServiceRequestMapper::configure(const char* conf) {
    BOOST_ASSERT(false);
    return -1;
}

int ServiceRequestMapper::configure(const std::string& conf) {
    BOOST_ASSERT(false);
    return -1;
}

int ServiceRequestMapper::configure(const ConfigCenter& confCenter) {
    BOOST_ASSERT(false);
    return -1;
}

int ServiceRequestMapper::configureFromFile(const std::string& file) {
    BOOST_ASSERT(false);
    return -1;
}

HttpServiceTemplate* ServiceRequestMapper::match(const HttpMethod& method, const std::vector<std::string>& pathSegments) {
    std::size_t j = serviceTemplates.size();

    for (std::size_t i = 0; i < j; ++i) {
        if (serviceTemplates[i].match(method, pathSegments)) {
            return &serviceTemplates[i];
        }
    }

    return NULL;
}

}
}
}
}
}
