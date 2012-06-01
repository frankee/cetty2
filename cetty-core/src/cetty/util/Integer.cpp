/**
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

#include <cetty/util/Integer.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace util {

Integer Integer::decode(const std::string& nm) {
    throw NotImplementedException("decode(const std::string& nm)");
}

cetty::util::Integer Integer::valueOf(int i) {
    return Integer(i);
}

cetty::util::Integer Integer::valueOf(const std::string& s) {
    return Integer(s);
}

cetty::util::Integer Integer::valueOf(const std::string& s, int radix) {
    throw NotImplementedException("valueOf");
}

int Integer::parse(const std::string& s) {
    return parse(s.c_str());
}

int Integer::parse(const std::string& s, int radix) {
    return parse(s.c_str(), radix);
}

int Integer::parse(const char* s) {
    BOOST_ASSERT(s); return atoi(s);
}

int Integer::parse(const char* s, int radix) {
    if (radix == 16) {
        unsigned int value;

        if (tryParseHex(s, value)) {
            return value;
        }
        else {
            throw SyntaxException("Not a valid integer", s);
        }
    }

    throw SyntaxException("Not a supported integer", s);
}

bool Integer::tryParse(const std::string& s, int& value) {
    char temp;
    return sscanf(s.c_str(), "%d%c", &value, &temp) == 1;
}

bool Integer::tryParseHex(const std::string& s, unsigned int& value) {
    return tryParseHex(s.c_str(), value);
}

bool Integer::tryParseHex(const char* s, unsigned int& value) {
    BOOST_ASSERT(s);
    char temp;
    return sscanf(s, "%x%c", &value, &temp) == 1;
}

std::string Integer::toBinaryString(boost::int64_t value) {
    throw NotImplementedException("toBinaryString");
}

std::string Integer::toBinaryString(boost::int64_t value, int width, bool zeroPadded /*= false*/) {
    throw NotImplementedException("toBinaryString");
}

std::string Integer::toHexString(boost::int64_t value) {
    std::string buf;
    appendHexString(value, &buf);
    return buf;
}

std::string Integer::toHexString(boost::int64_t value, int width, bool zeroPadded /*= false*/) {
    std::string buf;
    appendHexString(value, width, zeroPadded, &buf);
    return buf;
}

std::string Integer::toOctalString(boost::int64_t value) {
    throw NotImplementedException("toOctalString");
}

std::string Integer::toOctalString(boost::int64_t value, int width, bool zeroPadded /*= false*/) {
    throw NotImplementedException("toOctalString");
}

std::string Integer::toString(boost::int64_t value) {
    std::string buf;
    appendString(value, &buf);
    return buf;
}

std::string Integer::toString(boost::int64_t value, int width, bool zeroPadded /*= false*/) {
    std::string buf;
    appendString(value, width, zeroPadded, &buf);
    return buf;
}

void Integer::appendString(boost::int64_t value, std::string* str) {
    if (str) {
        char buf[64] = {0};
        sprintf(buf, "%lld", value);
        str->append(buf);
    }
}

void Integer::appendString(boost::int64_t value, int width, bool zeroPadded, std::string* str) {
    if (str) {
        BOOST_ASSERT(width > 0 && width < 64);
        char buffer[64];

        if (zeroPadded) {
            sprintf(buffer, "%0*lld", width, value);
        }
        else {
            sprintf(buffer, "%*lld", width, value);
        }

        str->append(buffer);
    }
}

void Integer::appendBinaryString(boost::int64_t value, std::string* str) {
    throw NotImplementedException("appendBinaryString");
}

void Integer::appendBinaryString(boost::int64_t value, int width, bool zeroPadded, std::string* str) {
    throw NotImplementedException("appendBinaryString");
}

void Integer::appendHexString(boost::int64_t value, std::string* str) {
    if (str) {
        char buffer[64] = {0};
        sprintf(buffer, "%X", value);
        str->append(buffer);
    }
}

void Integer::appendHexString(boost::int64_t value, int width, bool zeroPadded, std::string* str) {
    if (str) {
        BOOST_ASSERT(width > 0 && width < 64);
        char buffer[64] = {0};

        if (zeroPadded) {
            sprintf(buffer, "%0*X", width, value);
        }
        else {
            sprintf(buffer, "%*X", width, value);
        }

        str->append(buffer);
    }
}

void Integer::appendOctalString(boost::int64_t value, std::string* str) {
    throw NotImplementedException("appendOctalString");
}

void Integer::appendOctalString(boost::int64_t value, int width, bool zeroPadded, std::string* str) {
    throw NotImplementedException("appendOctalString");
}

}
}