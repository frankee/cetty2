/*
 * Copyright (cookie) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/handler/codec/http/ClientCookieEncoder.h>

#include <cetty/util/StringUtil.h>
#include <cetty/handler/codec/http/CookieHeaderNames.h>
#include <cetty/handler/codec/http/CookieEncoderUtil.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

static const char* const COOKIE_NAME_PREFIX = "$";

void ClientCookieEncoder::encode(const Cookie& cookie, std::string* out) {
    if (!out) {
        return;
    }

    if (cookie.getVersion() >= 1) {
        out->append(COOKIE_NAME_PREFIX);
        CookieEncoderUtil::add(CookieHeaderNames::COOKIE_VERSION, 1, out);
    }

    CookieEncoderUtil::add(cookie.getName(), cookie.getValue(), out);

    if (!cookie.getPath().empty()) {
        out->append(COOKIE_NAME_PREFIX);
        CookieEncoderUtil::add(CookieHeaderNames::COOKIE_PATH, cookie.getPath(), out);
    }

    if (!cookie.getDomain().empty()) {
        out->append(COOKIE_NAME_PREFIX);
        CookieEncoderUtil::add(CookieHeaderNames::COOKIE_DOMAIN, cookie.getDomain(), out);
    }

    if (cookie.getVersion() >= 1) {
        const std::vector<int>& ports = cookie.getPorts();

        if (!ports.empty()) {
            out->append(COOKIE_NAME_PREFIX);
            out->append(CookieHeaderNames::COOKIE_PORT);
            out->append(1, HttpCodecUtil::EQUALS);
            out->append(1, HttpCodecUtil::DOUBLE_QUOTE);

            for (std::size_t i = 0, j = ports.size(); i < j; ++i) {
                cetty::util::StringUtil::printf(out, "%d", ports[i]);
                out->append(1, HttpCodecUtil::COMMA);
            }

            (*out)[out->size() - 1] = HttpCodecUtil::DOUBLE_QUOTE;

            out->append(1, HttpCodecUtil::SEMICOLON);
            out->append(1, HttpCodecUtil::SP);
        }
    }

    CookieEncoderUtil::stripTrailingSeparator(out);
}

}
}
}
}
