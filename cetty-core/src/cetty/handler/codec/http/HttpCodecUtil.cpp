/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <boost/algorithm/string/predicate.hpp>

#include <cetty/handler/codec/http/HttpCodecUtil.h>
#include <cetty/handler/codec/http/HttpMessage.h>
#include <cetty/handler/codec/http/HttpHeader.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

const char HttpCodecUtil::CRLF[2] = { (char)HttpCodecUtil::CR, (char)HttpCodecUtil::LF };

//static final Charset DEFAULT_CHARSET = CharsetUtil.UTF_8;

bool HttpCodecUtil::validateHeaderName(const std::string& name) {
    for (size_t i = 0; i < name.size(); ++i) {
        char c = name.at(i);

        if (c > 127) {
            throw InvalidArgumentException(
                std::string("name contains non-ascii character: ") + name);
        }

        // Check prohibited characters.
        switch (c) {
        case '\t': case '\n': case 0x0b: case '\f': case '\r':
        case ' ':  case ',':  case ':':  case ';':  case '=':
            throw InvalidArgumentException(
                std::string("name contains one of the following prohibited characters: ") +
                std::string("=,;: \\t\\r\\n\\v\\f: ") + name);
        }
    }
}

bool HttpCodecUtil::validateHeaderValue(const std::string& value) {
    // 0 - the previous character was neither CR nor LF
    // 1 - the previous character was CR
    // 2 - the previous character was LF
    int state = 0;

    for (size_t i = 0; i < value.length(); ++i) {
        char c = value.at(i);

        // Check the absolutely prohibited characters.
        switch (c) {
        case 0x0b: // Vertical tab
            throw InvalidArgumentException(
                std::string("value contains a prohibited character '\\v': ") + value);

        case '\f':
            throw InvalidArgumentException(
                std::string("value contains a prohibited character '\\f': ") + value);
        }

        // Check the CRLF (HT | SP) pattern
        switch (state) {
        case 0:
            switch (c) {
            case '\r':
                state = 1;
                break;

            case '\n':
                state = 2;
                break;
            }

            break;

        case 1:
            switch (c) {
            case '\n':
                state = 2;
                break;

            default:
                throw InvalidArgumentException(
                    std::string("Only '\\n' is allowed after '\\r': ") + value);
            }

            break;

        case 2:
            switch (c) {
            case '\t': case ' ':
                state = 0;
                break;

            default:
                throw InvalidArgumentException(
                    std::string("Only ' ' and '\\t' are allowed after '\\n': ") + value);
            }
        }
    }

    if (state != 0) {
        throw InvalidArgumentException(
            std::string("value must not end with '\\r' or '\\n':") + value);
    }
}

bool HttpCodecUtil::isTransferEncodingChunked(const HttpMessage& m) {
    std::vector<std::string> chunked;
    m.getHeaders(HttpHeaders::Names::TRANSFER_ENCODING, &chunked);

    if (chunked.empty()) {
        return false;
    }

    for (size_t i = 0; i < chunked.size(); ++i) {
        if (boost::algorithm::iequals(HttpHeaders::Values::CHUNKED, chunked[i])) {
            return true;
        }
    }

    return false;
}
#if 0
void HttpCodecUtil::splitElements(const std::string& s, std::vector<std::string>& elements, bool ignoreEmpty)
{
    elements.clear();
    std::string::const_iterator it  = s.begin();
    std::string::const_iterator end = s.end();
    std::string elem;
    elem.reserve(64);
    while (it != end)
    {
        if (*it == '"')
        {
            elem += *it++;
            while (it != end && *it != '"')
            {
                if (*it == '\\')
                {
                    ++it;
                    if (it != end) elem += *it++;
                }
                else elem += *it++;
            }
            if (it != end) elem += *it++;
        }
        else if (*it == '\\')
        {
            ++it;
            if (it != end) elem += *it++;
        }
        else if (*it == ',')
        {
            Poco::trimInPlace(elem);
            if (!ignoreEmpty || !elem.empty())
                elements.push_back(elem);
            elem.clear();
            ++it;
        }
        else elem += *it++;
    }
    if (!elem.empty())
    {
        Poco::trimInPlace(elem);
        if (!ignoreEmpty || !elem.empty())
            elements.push_back(elem);
    }
}


void HttpCodecUtil::splitParameters(const std::string& s, std::string& value, NameValueCollection& parameters)
{
    value.clear();
    parameters.clear();
    std::string::const_iterator it  = s.begin();
    std::string::const_iterator end = s.end();
    while (it != end && Poco::Ascii::isSpace(*it)) ++it;
    while (it != end && *it != ';') value += *it++;
    Poco::trimRightInPlace(value);
    if (it != end) ++it;
    splitParameters(it, end, parameters);
}

void HttpCodecUtil::splitParameters(const std::string::const_iterator& begin, const std::string::const_iterator& end, NameValueCollection& parameters)
{
    std::string pname;
    std::string pvalue;
    pname.reserve(32);
    pvalue.reserve(64);
    std::string::const_iterator it = begin;
    while (it != end)
    {
        pname.clear();
        pvalue.clear();
        while (it != end && Poco::Ascii::isSpace(*it)) ++it;
        while (it != end && *it != '=' && *it != ';') pname += *it++;
        Poco::trimRightInPlace(pname);
        if (it != end && *it != ';') ++it;
        while (it != end && Poco::Ascii::isSpace(*it)) ++it;
        while (it != end && *it != ';')
        {
            if (*it == '"')
            {
                ++it;
                while (it != end && *it != '"')
                {
                    if (*it == '\\')
                    {
                        ++it;
                        if (it != end) pvalue += *it++;
                    }
                    else pvalue += *it++;
                }
                if (it != end) ++it;
            }
            else if (*it == '\\')
            {
                ++it;
                if (it != end) pvalue += *it++;
            }
            else pvalue += *it++;
        }
        Poco::trimRightInPlace(pvalue);
        if (!pname.empty()) parameters.add(pname, pvalue);
        if (it != end) ++it;
    }
}


void HttpCodecUtil::quote(const std::string& value, std::string& result, bool allowSpace)
{
    bool mustQuote = false;
    for (std::string::const_iterator it = value.begin(); !mustQuote && it != value.end(); ++it)
    {
        if (!Poco::Ascii::isAlphaNumeric(*it) && *it != '.' && *it != '_' && *it != '-' && !(Poco::Ascii::isSpace(*it) && allowSpace))
            mustQuote = true;
    }
    if (mustQuote) result += '"';
    result.append(value);
    if (mustQuote) result += '"';
}
#endif
}
}
}
}