#if !defined(CETTY_HTTP_URLTEMPLATE_H)
#define CETTY_HTTP_URLTEMPLATE_H

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
        int nextSameTypeIndex;
        int nextDiffTypeIndex;
        std::string name;
        std::string alias;
    };

public:
    UriTemplate(const std::string& templateStr);

    enum ParseState {
        ST_NORMAL,
        ST_PATH_MAP,
        ST_PATH_NORMAL,
        ST_PATH_PARAM,
        ST_LABEL,
        ST_QUERY
    };

    void parse(const std::string& str) {
        std::size_t j = str.size();
        int state;

        for (std::size_t i = 0; i < j; ++i) {
            switch (state) {
            case ST_NORMAL:
                if (str[i] == '[' && i < j - 1 && str[i+1] == '/') {
                    state = 1;

                }
                else if (str[i] == '/') {

                }
                else if (str[i] == '{' && i < j - 1) {
                    if (str[i+1] == '/') {
                    }
                    else if (str[i+1] == '.') {

                    }
                    else if (str[i+1] == '?' || str[i+1] == '&') {

                    }
                }
                break;

            case ST_PATH_MAP:
                break;

            case ST_PATH_NORMAL:
                break;

            case ST_PATH_PARAM:
                break;

            case ST_LABEL:
                break;

            case ST_QUERY:
                break;

            default:
                break;
            };
        }
    }

    //std::string expand(const NameValueCollection& values);
    //int extract(const std::string& uri, NameValueCollection* values);

    bool matchPath(const std::vector<std::string>& pathSegments) const {
        if (pathSegments.size() != this->pathSegments.size()) {
            return false;
        }

        int j = (int)this->pathSegments.size();

        for (int i = 0; i < j; ++i) {
            if (this->pathSegments[i].type == PATH_SEG_PARAM) {
                continue;
            }

            if (pathSegments[i].compare(this->pathSegments[i].name)) {
                return false;
            }
        }

        return true;
    }

    int getFirstPathAliasSegmentIndex() const;
    int getFirstPathParamSegmentIndex() const;

    const std::string& getLabelAlias() const;
    const std::string& getParameterAlias(const std::string& parameter) const;

private:
    //path
    std::vector<PathSegment> pathSegments;

    //label
    std::string label;
    std::string labelAlias;

    std::map<std::string, std::string> parameterAliases;
};

}
}

#endif //#if !defined(CETTY_HTTP_URLTEMPLATE_H)

// Local Variables:
// mode: c++
// End:
