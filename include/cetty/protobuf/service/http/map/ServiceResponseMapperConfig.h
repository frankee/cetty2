#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICERESPONSEMAPPERCONFIG_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICERESPONSEMAPPERCONFIG_H

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
#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::config;

class ServiceResponseMapperConfig : public cetty::config::ConfigObject {
public:
    class Template : public cetty::config::ConfigObject {
    public:
        class Header : public cetty::config::ConfigObject {
        public:
            std::string name;
            std::string value;

            virtual ConfigObject* create() const { return new Header; }
        };

        std::string service;
        std::string method;
        std::string content;
        std::vector<Header*> headers;

        virtual ConfigObject* create() const { return new Template; }
    };

    std::vector<Template*> templates;

    typedef std::vector<Template*> Templates;

    virtual ConfigObject* create() const { return new ServiceResponseMapperConfig; }
};

}
}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_SERVICERESPONSEMAPPERCONFIG_H)

// Local Variables:
// mode: c++
// End:
