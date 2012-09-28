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

#include <cetty/handler/codec/http/HttpHeaderDateFormat.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace boost::posix_time;

HttpHeaderDateFormat HttpHeaderDateFormat::cookieFormat("%A, %d-%b-%Y %H:%M:%S GMT");
HttpHeaderDateFormat HttpHeaderDateFormat::defaultFormat("%A, %d %b %Y %H:%M:%S GMT");

boost::posix_time::ptime HttpHeaderDateFormat::parse(const std::string& str) {
    ptime time;
    parse(str, &time);
    return time;
}

bool HttpHeaderDateFormat::parse(const std::string& str, ptime* time) {
    if (!str.empty() && time) {
        std::stringstream ss(str);
        ss.imbue(std::locale(ss.getloc(), &timeInput));

        ss >> *time;
        return true;
    }
    return false;
}

std::string HttpHeaderDateFormat::format(const ptime& time) {
    std::string tmp;
    format(time, &tmp);
    return tmp;
}

bool HttpHeaderDateFormat::format(const ptime& time, std::string* str) {
    if (str && !time.is_not_a_date_time()) {
        std::stringstream ss;
        ss.imbue(std::locale(ss.getloc(), &timeOutput));

        ss << time;
        *str = ss.str();
        return true;
    }
    return false;
}

}
}
}
}
