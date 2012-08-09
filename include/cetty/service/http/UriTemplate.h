#if !defined(CETTY_SERVICE_HTTP_URITEMPLATE_H)
#define CETTY_SERVICE_HTTP_URITEMPLATE_H

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
#include <cetty/util/NameValueCollection.h>

namespace cetty {
namespace service {
namespace http {

using namespace cetty::util;

// path
// [/method_path:alias]/path{/parameter:alias}{.format:alias}{?parameter1:alias}{&parameter2:alias}
// /method_path/path/parameter.format
// parameter1
// parameter2

class UriTemplate {
public:
    enum PathSegmentType {
        PATH_SEG_NORMAL = 0,
        PATH_SEG_ALIAS  = 1,
        PATH_SEG_PARAM  = 2,
    };

    struct PathSegment {
        int type;
        std::string name;
        std::string alias;
    };

    typedef std::vector<PathSegment> PathSegments;
    typedef std::map<std::string, std::string> QueryParams;

public:
    UriTemplate(const std::string& templateStr);

    void parse(const std::string& str);

    //std::string expand(const NameValueCollection& values);
    //int extract(const std::string& uri, NameValueCollection* values);

    bool matchPath(const std::vector<std::string>& pathSegments) const;

    const PathSegments& getPathSegments() const { return pathSegments; }
    
    const std::string& getLabel() const { return label; }
    const std::string& getLabelAlias() const { return labelAlias; }

    const QueryParams& getQueryParams() const { return queryParams; }

    const std::string& getParameterAlias(const std::string& parameter) const;

private:
    //path
    PathSegments pathSegments;

    //label
    std::string label;
    std::string labelAlias;

    //query
    QueryParams queryParams;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_HTTP_URITEMPLATE_H)

// Local Variables:
// mode: c++
// End:
