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

#include <cetty/handler/codec/http/QueryStringDecoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

static void decodeParams(const std::string& s, NameValueCollection& params) {
    std::string name;
    std::string::size_type pos = 0; // Beginning of the unprocessed region
    std::string::size_type i;       // End of the unprocessed region
    char c = 0;  // Current character

    for (i = 0; i < s.length(); ++i) {
        c = s[i];

        if (c == '=' && name.empty()) {
            if (pos != i) {
                name = s.substr(pos, i - pos);
            }

            //else if (pos == i), then have an empty name `=value'

            pos = i + 1;
        }
        else if (c == '&') {
            if (name.empty() && pos != i) {
                // We haven't seen an `=' so far but moved forward.
                // Must be a param of the form '&a&' so add it with
                // an empty value.
                params.add(s.substr(pos, i - pos), "");
            }
            else if (!name.empty()) {
                params.add(name, s.substr(pos, i - pos));
                name.clear();
            }

            pos = i + 1;
        }
    }

    if (pos != i) {  // Are there characters we haven't dealt with?
        if (name.empty()) {     // Yes and we haven't seen any `='.
            params.add(s.substr(pos, i - pos), "");
        }
        else {                // Yes and this must be the last value.
            params.add(name, s.substr(pos, i - pos));
        }
    }
    else if (!name.empty()) {  // Have we seen a name without value?
        params.add(name, "");
    }
}

QueryStringDecoder::QueryStringDecoder(const URI& uri)
    : uri(uri) {

}

void QueryStringDecoder::getParameters(NameValueCollection* params) const {
    if (params) {
        params->clear();

        if (!uri.getRawQuery().empty()) {
            decodeParams(uri.getQuery(), *params);
        }
    }
}

}
}
}
}
