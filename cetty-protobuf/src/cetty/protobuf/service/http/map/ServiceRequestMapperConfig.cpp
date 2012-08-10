
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

#include <cetty/protobuf/service/http/map/ServiceRequestMapperConfig.h>

#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

CETTY_CONFIG_ADD_DESCRIPTOR(ServiceRequestMapperConfig_Template,
                            new ConfigDescriptor(
                                3,
                                CETTY_CONFIG_FIELD(ServiceRequestMapperConfig::Template, uri, STRING),
                                CETTY_CONFIG_FIELD(ServiceRequestMapperConfig::Template, method, STRING),
                                CETTY_CONFIG_FIELD(ServiceRequestMapperConfig::Template, cookie, STRING)),
                            new ServiceRequestMapperConfig::Template);

CETTY_CONFIG_ADD_DESCRIPTOR(ServiceRequestMapperConfig,
                            new ConfigDescriptor(
                                1,
                                CETTY_CONFIG_REPEATED_OBJECT_FIELD(ServiceRequestMapperConfig, templates, Template)),
                            new ServiceRequestMapperConfig);


ServiceRequestMapperConfig::Template::Template()
    : ConfigObject("ServiceRequestMapperConfig_Template") {

}

ServiceRequestMapperConfig::ServiceRequestMapperConfig()
    : ConfigObject("ServiceRequestMapperConfig") {
}

}
}
}
}
}