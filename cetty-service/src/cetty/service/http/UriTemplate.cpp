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

#include <cetty/service/http/UriTemplate.h>

namespace cetty {
namespace service {
namespace http {

    enum ParseState {
        ST_NORMAL,
        ST_PATH_MAP,
        ST_PATH_NORMAL,
        ST_PATH_PARAM,
        ST_LABEL,
        ST_QUERY
    };

    void UriTemplate::parse(const std::string& str) {
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

    bool UriTemplate::matchPath(const std::vector<std::string>& pathSegments) const {
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

}
}
}
