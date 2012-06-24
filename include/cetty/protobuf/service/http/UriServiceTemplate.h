#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_URISERVICETEMPLATE_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_URISERVICETEMPLATE_H

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

#include <cetty/util/StringUtil.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {

using namespace cetty::util;

class UriServiceTemplate {
public:
    typedef std::vector<std::string> Alias;

public:
    UriServiceTemplate(const std::string& templateStr);


private:
    void parseAlias(const std::map<std::string, std::string>& aliases) {
        std::map<std::string, std::string>::const_iterator itr;

        for (itr = aliases.begin(); itr != aliases.end(); ++itr) {
            Alias& alias = parameterAliases[itr->first];
            parseAlias(itr->second, &alias);
        }
    }
    void parseAlias(const std::string& str, Alias* alias) {
        if (alias) {
            StringUtil::strsplit(str, '.', alias);
        }
    }

private:
    std::string service;
    std::string method;

    std::map<std::string, Alias> parameterAliases;
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_URISERVICETEMPLATE_H)

// Local Variables:
// mode: c++
// End:
