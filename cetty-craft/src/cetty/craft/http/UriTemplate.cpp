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

#include <cetty/craft/http/UriTemplate.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::util;

enum ParseState {
    ST_NORMAL,
    ST_PATH_MAP,
    ST_PATH_NORMAL,
    ST_PATH_PARAM,
    ST_LABEL,
    ST_QUERY
};

// [/method_path:alias]/path{/parameter:alias}{.format:alias}{?parameter1:alias}{&parameter2:alias}
// /method_path/path/parameter.format

static std::string::size_type retrieveSegment(const std::string& str,
        std::string::size_type start,
        char endChar,
        std::string* name,
        std::string* alias) {
    std::string::size_type stateEnd = str.find(endChar, start);

    if (stateEnd == str.npos) {
        return stateEnd;
    }

    std::string::size_type speratorPos = str.find(':', start);

    if (speratorPos == str.npos || speratorPos > stateEnd) {
        *name = str.substr(start + 1, stateEnd - start - 1);
        alias->clear();
    }
    else {
        *name = str.substr(start + 1, speratorPos - start - 1);
        *alias = str.substr(speratorPos + 1, stateEnd - speratorPos - 1);
    }

    return stateEnd;
}

static void appendPathSegments(const std::string& str,
                               UriTemplate::PathSegments* segments) {
    if (segments) {
        UriTemplate::PathSegment segment;
        std::vector<std::string> normalSegments;

        StringUtil::strsplit(str,
                             "/",
                             &normalSegments);

        std::size_t j = normalSegments.size();

        for (std::size_t i = 0; i < j; ++i) {
            const std::string& normalSegment = normalSegments[i];

            if (normalSegment.empty()) {
                continue;
            }

            segment.type = UriTemplate::PATH_SEG_NORMAL;
            segment.name = normalSegment;
            segment.alias.clear();
            segments->push_back(segment);
        }
    }
}

void UriTemplate::parse(const std::string& str) {
    std::size_t j = str.size();

    int state = ST_NORMAL;
    std::string::size_type stateBegin = 0;
    std::string::size_type stateEnd = 0;
    std::string::size_type speratorPos = 0;

    std::string query;
    std::string queryAlias;
    PathSegment segment;

    for (std::size_t i = 0; i < j; ++i) {
        switch (state) {
        case ST_NORMAL:
            if (str[i] == '[' && i < j - 1 && str[i+1] == '/') {
                state = ST_PATH_MAP;
                stateBegin = i + 1;
            }
            else if (str[i] == '/') {
                state = ST_PATH_NORMAL;
                stateBegin = i;
            }
            else if (str[i] == '{' && i < j - 1) {
                if (str[i+1] == '/') {
                    state = ST_PATH_PARAM;
                }
                else if (str[i+1] == '.') {
                    state = ST_LABEL;
                }
                else if (str[i+1] == '?' || str[i+1] == '&') {
                    state = ST_QUERY;
                }
                else {
                    // format error.
                    clear();
                }

                stateBegin = i + 1;
            }

            break;

        case ST_PATH_MAP:
            segment.type = PATH_SEG_ALIAS;
            stateEnd = retrieveSegment(str, stateBegin, ']', &segment.name, &segment.alias);

            if (stateEnd == str.npos) {
                clear();
                return;
            }

            pathSegments.push_back(segment);
            i = stateEnd;
            state = ST_NORMAL;
            break;

        case ST_PATH_NORMAL:
            stateEnd = str.find('[', stateBegin);

            if (stateEnd == str.npos) {
                stateEnd = str.find('{', stateBegin);

                if (stateEnd == str.npos) {
                    clear();
                    return;
                }
            }

            appendPathSegments(str.substr(stateBegin, stateEnd - stateBegin),
                               &pathSegments);
            i = stateEnd - 1;
            state = ST_NORMAL;
            break;

        case ST_PATH_PARAM:
            segment.type = PATH_SEG_PARAM;
            stateEnd = retrieveSegment(str, stateBegin, '}', &segment.name, &segment.alias);

            if (stateEnd == str.npos) {
                clear();
                return;
            }

            pathSegments.push_back(segment);
            i = stateEnd;
            state = ST_NORMAL;
            break;

        case ST_LABEL:
            stateEnd = retrieveSegment(str, stateBegin, '}', &label, &labelAlias);

            if (stateEnd == str.npos) {
                clear();
                return;
            }

            i = stateEnd;
            state = ST_NORMAL;
            break;

        case ST_QUERY:
            stateEnd = retrieveSegment(str, stateBegin, '}', &query, &queryAlias);

            if (stateEnd == str.npos) {
                clear();
                return;
            }

            if (!query.empty()) {
                queryParams.insert(std::make_pair(query, queryAlias));
            }

            i = stateEnd;
            state = ST_NORMAL;
            break;

        default:
            break;
        };
    }
}

bool UriTemplate::matchPath(const std::vector<std::string>& pathSegments) const {
    if (pathSegments.size() != this->pathSegments.size()) {
        return false;
    }

    std::size_t j = this->pathSegments.size();

    for (std::size_t i = 0; i < j; ++i) {
        if (this->pathSegments[i].type == PATH_SEG_PARAM) {
            continue;
        }

        if (pathSegments[i].compare(this->pathSegments[i].name)) {
            return false;
        }
    }

    return true;
}

UriTemplate::UriTemplate(const std::string& templateStr) {
    parse(templateStr);
}

void UriTemplate::clear() {
    pathSegments.clear();
    label.clear();
    labelAlias.clear();
    queryParams.clear();
}

}
}
}
