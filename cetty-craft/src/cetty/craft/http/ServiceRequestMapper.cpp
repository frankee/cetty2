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

#include <cetty/craft/http/ServiceRequestMapper.h>

#include <cetty/config/ConfigCenter.h>


namespace cetty {
namespace craft {
namespace http {

ServiceRequestMapper::ServiceRequestMapper() {
    ConfigCenter::instance().configure(&config);
    init();
}

ServiceRequestMapper::ServiceRequestMapper(const std::string& conf) {
    ConfigCenter::configureFromString(conf, &config);
    init();
}

ServiceRequestMapper::~ServiceRequestMapper() {

}

bool ServiceRequestMapper::configure(const char* conf) {
    if (ConfigCenter::configureFromString(conf, &config)) {
        return init();
    }
    return false;
}

bool ServiceRequestMapper::configure(const std::string& conf) {
    if (ConfigCenter::configureFromString(conf, &config)) {
        return init();
    }
    return false;
}

bool ServiceRequestMapper::configure(const ServiceRequestMapperConfig& conf) {
    config.copyFrom(conf);
    return init();
}

bool ServiceRequestMapper::configureFromFile(const std::string& file) {
    if (ConfigCenter::configureFromFile(file, &config)) {
        return init();
    }
    return false;
}

HttpRequestTemplate* ServiceRequestMapper::match(const HttpMethod& method, const std::vector<std::string>& pathSegments) {
    std::size_t j = serviceTemplates.size();

    for (std::size_t i = 0; i < j; ++i) {
        if (serviceTemplates[i]->match(method, pathSegments)) {
            return serviceTemplates[i];
        }
    }

    return NULL;
}

bool ServiceRequestMapper::init() {
    deinit();

    std::size_t j = config.templates.size();
    for (std::size_t i = 0; i < j; ++i) {
        const ServiceRequestMapperConfig::Template* t = config.templates[i];
        if (t) {
            HttpRequestTemplate* templ = new HttpRequestTemplate(t->method, t->uri, t->cookie);

            if (templ->validated()) {
                serviceTemplates.push_back(templ);
            }
            else {
                delete templ;
            }
        }
    }

    return !serviceTemplates.empty();
}

void ServiceRequestMapper::deinit() {
    std::size_t j = serviceTemplates.size();
    for (std::size_t i = 0; i < j; ++i) {
        HttpRequestTemplate* tmpl = serviceTemplates[i];
        if (tmpl) {
            delete tmpl;
        }
    }

    serviceTemplates.clear();
}

}
}
}
