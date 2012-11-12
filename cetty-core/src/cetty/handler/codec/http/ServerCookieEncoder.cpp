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

#include <cetty/handler/codec/http/ServerCookieEncoder.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <cetty/util/StringUtil.h>
#include <cetty/handler/codec/http/CookieHeaderNames.h>
#include <cetty/handler/codec/http/CookieEncoderUtil.h>
#include <cetty/handler/codec/http/HttpHeaderDateFormat.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace boost::posix_time;

void ServerCookieEncoder::encode(const Cookie& cookie, std::string* out) {
    if (!out) {
        return;
    }

    CookieEncoderUtil::add(cookie.getName(), cookie.getValue(), out);

    if (cookie.getMaxAge() >= 0) {
        if (cookie.getVersion() == 0) {
            CookieEncoderUtil::addUnquoted(
                CookieHeaderNames::COOKIE_EXPIRES,
                HttpHeaderDateFormat::cookieFormat.format(
                    second_clock::universal_time() + seconds(cookie.getMaxAge())),
                out);
        }
        else {
            CookieEncoderUtil::add(CookieHeaderNames::COOKIE_MAX_AGE,
                                   cookie.getMaxAge(),
                                   out);
        }
    }

    if (!cookie.getPath().empty()) {
        if (cookie.getVersion() > 0) {
            CookieEncoderUtil::add(CookieHeaderNames::COOKIE_PATH,
                                   cookie.getPath(),
                                   out);
        }
        else {
            CookieEncoderUtil::addUnquoted(CookieHeaderNames::COOKIE_PATH,
                                           cookie.getPath(),
                                           out);
        }
    }

    if (!cookie.getDomain().empty()) {
        if (cookie.getVersion() > 0) {
            CookieEncoderUtil::add(CookieHeaderNames::COOKIE_DOMAIN,
                                   cookie.getDomain(),
                                   out);
        }
        else {
            CookieEncoderUtil::addUnquoted(CookieHeaderNames::COOKIE_DOMAIN,
                                           cookie.getDomain(),
                                           out);
        }
    }

    if (cookie.isSecure()) {
        out->append(CookieHeaderNames::COOKIE_SECURE);
        out->append(1, HttpCodecUtil::SEMICOLON);
        out->append(1, HttpCodecUtil::SP);
    }

    if (cookie.isHttpOnly()) {
        out->append(CookieHeaderNames::COOKIE_HTTP_ONLY);
        out->append(1, HttpCodecUtil::SEMICOLON);
        out->append(1, HttpCodecUtil::SP);
    }

    if (cookie.getVersion() >= 1) {
        if (!cookie.getComment().empty()) {
            CookieEncoderUtil::add(CookieHeaderNames::COOKIE_COMMENT,
                                   cookie.getComment(),
                                   out);
        }

        CookieEncoderUtil::add(CookieHeaderNames::COOKIE_VERSION, 1, out);

        if (!cookie.getCommentUrl().empty()) {
            CookieEncoderUtil::addQuoted(CookieHeaderNames::COOKIE_COMMENT_URL,
                                         cookie.getCommentUrl(),
                                         out);
        }

        if (!cookie.getPorts().empty()) {
            const std::vector<int>& ports = cookie.getPorts();
            out->append(CookieHeaderNames::COOKIE_PORT);
            out->append(1, HttpCodecUtil::EQUALS);
            out->append(1, HttpCodecUtil::DOUBLE_QUOTE);

            for (std::size_t i = 0, j = ports.size(); i < j; ++i) {
                StringUtil::printf(out, "%d", ports[i]);
                out->append(1, HttpCodecUtil::COMMA);
            }

            (*out)[out->length() - 1] = HttpCodecUtil::DOUBLE_QUOTE;
            out->append(1, HttpCodecUtil::SEMICOLON);
            out->append(1, HttpCodecUtil::SP);
        }

        if (cookie.isDiscard()) {
            out->append(CookieHeaderNames::COOKIE_DISCARD);
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
