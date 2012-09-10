#if !defined(CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPERCONFIG_H)
#define CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPERCONFIG_H

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

#include <cetty/config/ConfigObject.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::config;

class ServiceRequestMapperConfig : public cetty::config::ConfigObject {
public:
    class Template : public cetty::config::ConfigObject {
    public:
        std::string uri;
        std::string method;
        std::string cookie;

        Template();

        virtual ConfigObject* create() const {
            return new Template;
        }
    };

    std::vector<Template*> templates;

    ServiceRequestMapperConfig();

    virtual ConfigObject* create() const { return new ServiceRequestMapperConfig; }
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_HTTP_SERVICEREQUESTMAPPERCONFIG_H)

// Local Variables:
// mode: c++
// End:
