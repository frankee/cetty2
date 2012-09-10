#if !defined(CETTY_CRAFT_BUILDER_RESTHTTPCLIENTBUILDER_H)
#define CETTY_CRAFT_BUILDER_RESTHTTPCLIENTBUILDER_H

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

#include <cetty/bootstrap/ClientBootstrap.h>

namespace cetty {
namespace craft {
namespace builder {

class RestHttpResponse {

};

class RestHttpRequest {
public:
    RestHttpRequest(const std::string& tmpl);

    void setPathParam(const std::string& param, const std::string& value);
    void setQueryParam(const std::string& param, const std::string& value);
    void setFormat(const std::string& format);

    void header(const std::string& key, const std::string& value);
    void body(const std::string& type, const std::string& value);

    RestHttpResponse post();
    RestHttpResponse get();
};

class RestHttpClientBuilder {

};

}
}
}

#endif //#if !defined(CETTY_CRAFT_BUILDER_RESTHTTPCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
