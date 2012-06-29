#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_HTTPSERVICETEMPLATE_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_MAP_HTTPSERVICETEMPLATE_H

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

#include <string>
#include <vector>
#include <map>
#include <cetty/http/UriTemplate.h>
#include <cetty/http/CookieTemplate.h>
#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/SimpleTrie.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::http;
using namespace cetty::util;
using namespace cetty::handler::codec::http;

// GET uri template
class HttpServiceTemplate {
public:
    struct Parameter {
        enum Type {
            T_PATH   = 1,
            T_QUERY  = 2,
            T_COOKIE = 3
        };

        bool isInPath() const { return type == T_PATH; }
        bool isInQuery() const { return type == T_QUERY; }
        bool isInCookie() const { return type == T_COOKIE; }

        int type;
        int index;
        std::string cookie;
        std::string name;
    };

public:
    HttpServiceTemplate();

    bool match(const HttpMethod& method, const std::vector<std::string>& pathSegments) const {
        return method == httpMethod && uriTemplate.matchPath(pathSegments);
    }

    const std::string& getService() const { return service; }
    const std::string& getMethod() const { return method; }

    bool hasField(const std::string& field) const {
        return trie.countPrefix(field) > 0;
    }

    const Parameter* getParameter(const std::string& field) const {
        void* data = trie.getValue(field);
        return data ? (const Parameter*)data : NULL;
    }

private:
    HttpMethod httpMethod;
    UriTemplate uriTemplate;
    CookieTemplate cookieTemplate;

    // parsed data.
    std::string service;
    std::string method;
    SimpleTrie trie;
};

}
}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_HTTPSERVICETEMPLATE_H)

// Local Variables:
// mode: c++
// End:
