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

#include <cetty/handler/codec/http/CookieDecoder.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <cetty/util/StringUtil.h>
#include <cetty/handler/codec/http/Cookie.h>
#include <cetty/handler/codec/http/CookieHeaderNames.h>
#include <cetty/handler/codec/http/HttpHeaderDateFormat.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

void CookieDecoder::decode(const std::string& header, std::vector<Cookie>* cookies) {
    std::vector<std::string> names;
    std::vector<std::string> values;
    extractKeyValuePairs(header, &names, &values);

    if (names.empty()) {
        return;
    }

    int i;
    int version = 0;

    // $Version is the only attribute that can appear before the actual
    // cookie name-value pair.
    if (StringUtil::striequals(names[0].c_str(),
                               CookieHeaderNames::COOKIE_VERSION.c_str())) {
        version = (int)StringUtil::atoi(values.at(0));
        i = 1;
    }
    else {
        i = 0;
    }

    if (names.size() <= i) {
        // There's a version attribute, but nothing more.
        return;
    }

    for (; i < names.size(); i ++) {
        Cookie c(names.at(i), values.at(i));

        bool discard = false;
        bool secure = false;
        bool httpOnly = false;
        std::string comment;
        std::string commentURL;
        int maxAge = -1;
        std::vector<int> ports;

        for (int j = i + 1; j < names.size(); j++, i++) {
            std::string& name = names.at(j);
            std::string& value = values.at(j);

            if (StringUtil::striequals(
                        CookieHeaderNames::COOKIE_DISCARD.c_str(), name.c_str())) {
                discard = true;
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_SECURE.c_str(), name.c_str())) {
                secure = true;
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_HTTP_ONLY.c_str(), name.c_str())) {
                httpOnly = true;
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_COMMENT.c_str(), name.c_str())) {
                comment = value;
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_COMMENT_URL.c_str(), name.c_str())) {
                commentURL = value;
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_DOMAIN.c_str(), name.c_str())) {
                c.setDomain(value);
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_PATH.c_str(), name.c_str())) {
                c.setPath(value);
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_EXPIRES.c_str(), name.c_str())) {
                boost::posix_time::ptime time;
                HttpHeaderDateFormat::defaultFormat.parse(value, &time);

                boost::posix_time::time_duration duration =
                    time - boost::posix_time::microsec_clock::universal_time();
                int64_t maxAgeMillis = duration.total_milliseconds();

                if (maxAgeMillis <= 0) {
                    maxAge = 0;
                }
                else {
                    maxAge = (int)(maxAgeMillis / 1000 + (maxAgeMillis % 1000 != 0 ? 1 : 0));
                }
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_MAX_AGE.c_str(), name.c_str())) {
                maxAge = (int)StringUtil::atoi(value);
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_VERSION.c_str(), name.c_str())) {
                version = (int)StringUtil::atoi(value);
            }
            else if (StringUtil::striequals(
                         CookieHeaderNames::COOKIE_PORT.c_str(), name.c_str())) {

                std::vector<std::string> portList;
                StringUtil::strsplit(value, ',', &portList);

                for (std::size_t i = 0, j = portList.size(); i < j; ++i) {
                    ports.push_back((int)StringUtil::atoi(portList[i]));
                }
            }
            else {
                break;
            }
        }

        c.setVersion(version);
        c.setMaxAge(maxAge);
        c.setSecure(secure);
        c.setHttpOnly(httpOnly);

        if (version > 0) {
            c.setComment(comment);
        }

        if (version > 1) {
            c.setCommentUrl(commentURL);
            c.setPorts(ports);
            c.setDiscard(discard);
        }

        cookies->push_back(c);
    }
}

void CookieDecoder::extractKeyValuePairs(const std::string& header,
        std::vector<std::string>* names,
        std::vector<std::string>* values) {
    std::string::size_type headerLen = header.size();

    for (std::string::size_type i = 0; i < headerLen;) {
        // Skip spaces and separators.
        for (; i < headerLen;) {
            switch (header.at(i)) {
            case '\t': case '\n': case 0x0b: case '\f': case '\r':
            case ' ':  case ',':  case ';':
                i ++;
                continue;
            }

            break;
        }

        // Skip '$'.
        for (; i < headerLen;) {
            if (header.at(i) == '$') {
                i ++;
                continue;
            }

            break;
        }

        std::string name;
        std::string value;

        int newNameStart = i;
        bool valueParsed = false;

        for (;;) {
            int newValueStart;
            char c;

            switch (header.at(i)) {
            case ';':
                // NAME; (no value till ';')
                name = header.substr(newNameStart, i - newNameStart);
                value.clear();
                break;

            case '=':
                // NAME=VALUE
                name = header.substr(newNameStart, i - newNameStart);
                i ++;

                if (i == headerLen) {
                    // NAME= (empty value, i.e. nothing after '=')
                    value.clear();
                    break;
                }

                newValueStart = i;
                c = header.at(i);

                if (c == '"' || c == '\'') {
                    // NAME="VALUE" or NAME='VALUE'
                    char q = c;
                    bool hasBackslash = false;
                    i ++;

                    for (; i < headerLen;) {
                        if (hasBackslash) {
                            hasBackslash = false;
                            c = header.at(i ++);

                            switch (c) {
                            case '\\': case '"': case '\'':
                                // Escape last backslash.
                                value[value.size() - 1] = c;
                                break;

                            default:
                                // Do not escape last backslash.
                                value.append(1, c);
                            }
                        }
                        else {
                            c = header.at(i ++);

                            if (c == q) {
                                valueParsed = true;
                                break;
                            }

                            value.append(1, c);

                            if (c == '\\') {
                                hasBackslash = true;
                            }
                        }
                    }

                    if (valueParsed) {
                        valueParsed = false;
                        break;
                    }
                }
                else {
                    // NAME=VALUE;
                    std::string::size_type semiPos = header.find(';', i);

                    if (semiPos != header.npos) {
                        value = header.substr(newValueStart, semiPos - newValueStart);
                        i = semiPos;
                    }
                    else {
                        value = header.substr(newValueStart);
                        i = headerLen;
                    }
                }

                break;

            default:
                i ++;
            }

            if (i == headerLen) {
                // NAME (no value till the end of string)
                name = header.substr(newNameStart);
                value.clear();
                break;
            }
        }

        names->push_back(name);
        values->push_back(value);
    }
}

}
}
}
}
