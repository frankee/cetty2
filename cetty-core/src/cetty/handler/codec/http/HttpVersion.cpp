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

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/StringPiece.h>

#include <cetty/handler/codec/http/HttpVersion.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

cetty::handler::codec::http::HttpVersion HttpVersion::HTTP_1_0("HTTP", 1, 0, false);
cetty::handler::codec::http::HttpVersion HttpVersion::HTTP_1_1("HTTP", 1, 1, true);

cetty::handler::codec::http::HttpVersion HttpVersion::valueOf(const std::string& text) {
    std::string version = boost::trim_copy(text);
    boost::to_upper(version);

    if (version == "HTTP/1.1") {
        return HTTP_1_1;
    }

    if (version == "HTTP/1.0") {
        return HTTP_1_0;
    }

    return HttpVersion(version, true);
}

HttpVersion HttpVersion::valueOf(const StringPiece& text) {
    StringPiece version = text.trim();

    if (version.iequals("HTTP/1.1")) {
        return HTTP_1_1;
    }

    if (version.iequals("HTTP/1.0")) {
        return HTTP_1_0;
    }

    BOOST_ASSERT(false && "no such http version");
    //return HttpVersion(version, true);
}


HttpVersion::HttpVersion(const std::string& text, bool keepAliveDefault)
    : keepAliveDefault(keepAliveDefault),
      majorVersion(0),
      minorVersion(0) {
    std::string version = boost::trim_copy(text);
    boost::to_upper(version);

    if (version.size() == 0) {
        throw InvalidArgumentException("empty text");
    }

    std::string major;
    std::string minor;

    if (!match(version, protocolName, major, minor)) {
        throw InvalidArgumentException(std::string("invalid version format: ") + text);
    }

    majorVersion = StringUtil::strto32(major);
    minorVersion = StringUtil::strto32(minor);
    this->text = protocolName + "/" + major + "." + minor;
}

HttpVersion::HttpVersion(const std::string& protocolName,
                         int majorVersion,
                         int minorVersion,
                         bool keepAliveDefault)
    : keepAliveDefault(keepAliveDefault),
      majorVersion(majorVersion),
      minorVersion(minorVersion),
      protocolName(protocolName) {

    this->protocolName = boost::trim_copy(protocolName);
    boost::to_upper(this->protocolName);

    if (this->protocolName.empty()) {
        throw InvalidArgumentException("empty protocolName");
    }

    if (!verifyProtocl(this->protocolName)) {
        throw InvalidArgumentException("invalid protocalName");
    }

    if (majorVersion < 0) {
        throw InvalidArgumentException("negative majorVersion");
    }

    if (minorVersion < 0) {
        throw InvalidArgumentException("negative minorVersion");
    }

    StringUtil::printf(&text,
                          "%s/%d.%d",
                          protocolName.c_str(),
                          majorVersion,
                          minorVersion);
}

int HttpVersion::hashCode() const {
    return (StringUtil::hashCode(getProtocolName()) * 31 + getMajorVersion()) * 31 +
           getMinorVersion();
}

bool HttpVersion::equals(const HttpVersion& verion) const {
    return getMinorVersion() == verion.getMinorVersion() &&
           getMajorVersion() == verion.getMajorVersion() &&
           getProtocolName() == verion.getProtocolName();
}

int HttpVersion::compareTo(const HttpVersion& version) const {
    int v = protocolName.compare(version.protocolName);

    if (v != 0) {
        return v;
    }

    v = getMajorVersion() - version.getMajorVersion();

    if (v != 0) {
        return v;
    }

    return getMinorVersion() - version.getMinorVersion();
}

bool HttpVersion::match(const std::string& text, std::string& protocol, std::string& major, std::string& minor) {
    std::string::size_type slashPos = text.find('/');

    if (std::string::npos == slashPos) { return false; }

    protocol = text.substr(0, slashPos);

    if (!verifyProtocl(protocol)) { return false; }

    std::string::size_type dotPos = text.find(slashPos, '.');

    if (std::string::npos == dotPos) { return false; }

    major = text.substr(slashPos + 1, dotPos);
    minor = text.substr(dotPos + 1);

    if (!verifyVersion(major) || !verifyVersion(minor)) { return false; }

    return true;
}

bool HttpVersion::verifyProtocl(const std::string& protocolName) {
    for (size_t i = 0; i < protocolName.size(); ++i) {
        if (StringUtil::isISOControl(protocolName[i])
                || StringUtil::isWhitespace(protocolName[i])) {
            return false;
        }
    }

    return true;
}

bool HttpVersion::verifyVersion(const std::string& version) {
    for (size_t i = 0; i < version.size(); ++i) {
        if (!StringUtil::isDigit(version[i])) {
            return false;
        }
    }

    return true;
}

}
}
}
}
