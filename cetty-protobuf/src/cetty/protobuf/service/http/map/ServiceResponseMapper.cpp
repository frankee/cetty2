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

#include <cetty/protobuf/service/http/map/ServiceResponseMapper.h>
#include <cetty/handler/codec/http/HttpResponse.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::handler::codec::http;


ServiceResponseMapper::ServiceResponseMapper() {

}

ServiceResponseMapper::ServiceResponseMapper(const std::string& conf) {

}

ServiceResponseMapper::ServiceResponseMapper(const ConfigCenter& confCenter) {

}

int ServiceResponseMapper::configure(const std::string& conf) {
    return 0;
}

int ServiceResponseMapper::configure(const ConfigCenter& confCenter) {
    return 0;
}

int ServiceResponseMapper::configureFromFile(const std::string& file) {
    return 0;
}

const ServiceResponseMapper::MapValue* ServiceResponseMapper::match(
    const std::string& service,
    const std::string& method) const {
        return NULL;
}

void ServiceResponseMapper::setHttpHeaders(const MapValue& value,
        const HttpResponsePtr& response) {
    std::map<std::string, std::string>::const_iterator itr = value.headers.begin();
    std::map<std::string, std::string>::const_iterator end = value.headers.end();

    for (; itr != end; ++itr) {
        response->setHeader(itr->first, itr->second);
    }
}

//ServiceResponseMapperConfig repConfig;
//ServiceRequestMapperConfig reqConfig;
//config.configure(&reqConfig);
//config.configure(&repConfig);

}
}
}
}
}