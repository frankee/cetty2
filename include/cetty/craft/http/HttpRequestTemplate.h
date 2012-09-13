#if !defined(CETTY_CRAFT_HTTP_HTTPREQUESTTEMPLATE_H)
#define CETTY_CRAFT_HTTP_HTTPREQUESTTEMPLATE_H

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
#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/SimpleTrie.h>
#include <cetty/craft/http/UriTemplate.h>
#include <cetty/craft/http/CookieTemplate.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::util;
using namespace cetty::handler::codec::http;

// GET uri template
class HttpRequestTemplate {
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
        std::string alias;

        Parameter(int type, const std::string& name, const std::string& alias)
            : type(type), name(name), alias(alias), index(0) {
        }
        Parameter(int type, const std::string& name, const std::string& alias, int index)
            : type(type), name(name), alias(alias), index(index) {
        }

        static Parameter* createFromPath(const std::string& name, const std::string& alias, int index) {
            return new Parameter(T_PATH, name, alias, index);
        }
        static Parameter* createFromQuery(const std::string& name, const std::string& alias) {
            return new Parameter(T_QUERY, name, alias);
        }
        static Parameter* createFromCookie();
    };

public:
    HttpRequestTemplate(const std::string& method,
                        const std::string& uri,
                        const std::string& cookie);

    bool validated() const;

    bool match(const HttpMethod& method, const std::vector<std::string>& pathSegments) const {
        return method == httpMethod && uriTemplate.matchPath(pathSegments);
    }

    const std::string& getService() const { return service; }
    const std::string& getMethod() const { return method; }

    bool hasField(const std::string& field) const {
        return trie.count(field) > 0;
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

#endif //#if !defined(CETTY_CRAFT_HTTP_HTTPREQUESTTEMPLATE_H)

// Local Variables:
// mode: c++
// End:
