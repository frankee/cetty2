
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

#include <cetty/craft/http/ServiceResponseMapperConfig.h>

namespace cetty {
namespace craft {
namespace http {

CETTY_CONFIG_ADD_DESCRIPTOR(ServiceResponseMapperConfig_Template,
                            new ConfigDescriptor(
                                4,
                                CETTY_CONFIG_FIELD(ServiceResponseMapperConfig::Template, service, STRING),
                                CETTY_CONFIG_FIELD(ServiceResponseMapperConfig::Template, method, STRING),
                                CETTY_CONFIG_FIELD(ServiceResponseMapperConfig::Template, content, STRING),
                                CETTY_CONFIG_REPEATED_OBJECT_FIELD(ServiceResponseMapperConfig::Template, headers, KeyValuePair)),
                            new ServiceResponseMapperConfig::Template);

CETTY_CONFIG_ADD_DESCRIPTOR(ServiceResponseMapperConfig,
                            new ConfigDescriptor(
                                1,
                                CETTY_CONFIG_REPEATED_OBJECT_FIELD(ServiceResponseMapperConfig, templates, Template)),
                            new ServiceResponseMapperConfig);


ServiceResponseMapperConfig::Template::Template()
    : ConfigObject("ServiceResponseMapperConfig_Template") {
}

ServiceResponseMapperConfig::ServiceResponseMapperConfig()
    : ConfigObject("ServiceResponseMapperConfig") {
}

}
}
}
