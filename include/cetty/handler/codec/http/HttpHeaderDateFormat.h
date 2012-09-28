#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPHEADERDATEFORMAT_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPHEADERDATEFORMAT_H
/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <string>
#include <sstream>
#include <locale>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using boost::posix_time::ptime;
using boost::posix_time::time_facet;
using boost::posix_time::time_input_facet;

/**
 * This DateFormat decodes 3 formats of {@link Date}, but only encodes the one,
 * the first:
 * <ul>
 * <li>Sun, 06 Nov 1994 08:49:37 GMT: standard specification, the only one with
 * valid generation</li>
 * <li>Sun, 06 Nov 1994 08:49:37 GMT: obsolete specification</li>
 * <li>Sun Nov 6 08:49:37 1994: obsolete specification</li>
 * </ul>
 */
class HttpHeaderDateFormat {
public:
    /**
     * Standard date format<p>
     * Sun, 06 Nov 1994 08:49:37 GMT -> E, d MMM yyyy HH:mm:ss z
     */
    HttpHeaderDateFormat()
        : formatStr("%A, %d %b %Y %H:%M:%S GMT"),
          timeOutput(formatStr.c_str()),
          timeInput(formatStr.c_str()) {
    }

    HttpHeaderDateFormat(const std::string& format)
        : formatStr(format),
          timeOutput(format.c_str()),
          timeInput(format.c_str()) {
    }

    ptime parse(const std::string& str);
    bool parse(const std::string& str, ptime* time);

    std::string format(const ptime& time);
    bool format(const ptime& time, std::string* str);

    /**
     * Cookie date format<p>
     * Sun, 06-Nov-1994 08:49:37 GMT -> E, DD-MMM-YYYY HH:mm:ss z
     */
    static HttpHeaderDateFormat defaultFormat;

    /**
     * Head date format HTTP1.1
     * <p>
     * Sun, 06 Nov 1994 08:49:37 GMT
     */
    static HttpHeaderDateFormat cookieFormat;

private:
    std::string formatStr;

    time_facet timeOutput;
    time_input_facet timeInput;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPHEADERDATEFORMAT_H)

// Local Variables:
// mode: c++
// End:
