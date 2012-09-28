#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEENCODERUTIL_H)
#define CETTY_HANDLER_CODEC_HTTP_COOKIEENCODERUTIL_H
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

#include <boost/algorithm/string/replace.hpp>
#include <cetty/Types.h>
#include <cetty/util/StringUtil.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class CookieEncoderUtil {
public:
    static void stripTrailingSeparator(std::string* buf) {
        if (buf && !buf->empty()) {
            buf->resize(buf->length() - 2);
        }
    }

    static void add(const std::string& name,
                    const std::string& val,
                    std::string* out) {
        if (out) {
            if (val.empty()) {
                addQuoted(name, "", out);
                return;
            }

            for (std::size_t i = 0; i < val.length(); ++i) {
                switch (val[i]) {
                case '\t': case ' ': case '"': case '(':  case ')': case ',':
                case '/':  case ':': case ';': case '<':  case '=': case '>':
                case '?':  case '@': case '[': case '\\': case ']':
                case '{':  case '}':
                    addQuoted(name, val, out);
                    return;
                }
            }

            addUnquoted(name, val, out);
        }
    }

    static void addUnquoted(const std::string& name,
                            const std::string& val,
                            std::string* out) {
        if (out) {
            out->append(name);
            out->append(1, HttpCodecUtil::EQUALS);
            out->append(val);
            out->append(1, HttpCodecUtil::SEMICOLON);
            out->append(1, HttpCodecUtil::SP);

        }

    }

    static void addQuoted(const std::string& name,
                          const std::string& val,
                          std::string* out) {
        if (out) {
            std::string value(val);

            out->append(name);
            out->append(1, HttpCodecUtil::EQUALS);
            out->append(1, HttpCodecUtil::DOUBLE_QUOTE);

            boost::replace_all(value, "\\", "\\\\");
            boost::replace_all(value, "\"", "\\\"");
            out->append(value);

            out->append(1, HttpCodecUtil::DOUBLE_QUOTE);
            out->append(1, HttpCodecUtil::SEMICOLON);
            out->append(1, HttpCodecUtil::SP);
        }

    }

    static void add(const std::string& name, int val, std::string* out) {
        if (out) {
            out->append(name);
            out->append(1, HttpCodecUtil::EQUALS);

            cetty::util::StringUtil::strprintf(out, "%d", val);

            out->append(1, HttpCodecUtil::SEMICOLON);
            out->append(1, HttpCodecUtil::SP);
        }
    }

private:
    CookieEncoderUtil() {}
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_COOKIEENCODERUTIL_H)

// Local Variables:
// mode: c++
// End:
