/**
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

#include <cetty/craft/http/ServiceResponseMapper.h>
#include <cetty/handler/codec/http/HttpResponse.h>

#include <cetty/config/ConfigCenter.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::handler::codec::http;

ServiceResponseMapper::ServiceResponseMapper() {
    ConfigCenter::instance().configure(&config);
    init();
}

ServiceResponseMapper::ServiceResponseMapper(const std::string& conf) {
    ConfigCenter::configureFromString(conf, &config);
    init();
}

bool ServiceResponseMapper::configure(const std::string& conf) {
    if (ConfigCenter::configureFromString(conf, &config)) {
        return init();
    }
    return false;
}

bool ServiceResponseMapper::configureFromFile(const std::string& file) {
    if (ConfigCenter::configureFromFile(file, &config)) {
        return init();
    }
    
    return false;
}

const ServiceResponseMapper::ResponseTemplate* ServiceResponseMapper::match(
    const std::string& service,
    const std::string& method) const {
    std::string key(service);
    key += method;

    ResponseTemplateMap::const_iterator itr = maps.find(key);

    if (itr != maps.end()) {
        return itr->second;
    }

    return NULL;
}

void ServiceResponseMapper::setHttpHeaders(const ResponseTemplate& templ,
        const HttpResponsePtr& response) {

    std::size_t j = templ.headers.size();

    for (std::size_t i = 0; i < j; ++i) {
        const ResponseTemplate::Header* header = templ.headers[i];
        response->setHeader(header->name, header->value);
    }
}

bool ServiceResponseMapper::init() {
    maps.clear();

    std::size_t j = config.templates.size();

    for (std::size_t i = 0; i < j; ++i) {
        const ServiceResponseMapperConfig::Template* tmpl = config.templates[i];
        std::string key(tmpl->service);
        key += tmpl->method;

        maps[key] = tmpl;
    }

    return !maps.empty();
}

}
}
}
