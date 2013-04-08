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

#include <cetty/channel/InetAddress.h>

#include <boost/assert.hpp>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

const InetAddress InetAddress::EMPTY;

static const std::string ANY_IPV4 = "0.0.0.0";
static const std::string ANY_IPV6 = "0.0.0.0";

InetAddress::InetAddress()
    : port_(0),
      family_(FAMILY_NONE) {
}

InetAddress::InetAddress(int family)
    : port_(0),
      family_(family) {
    if (!checkFamily(family)) {
        family_ = FAMILY_NONE;
    }
}

InetAddress::InetAddress(int family, int port)
    : port_(port),
      family_(family) {
    if (!checkFamily(family) || !checkPort(port)) {
        port_ = 0;
        family_ = FAMILY_NONE;
    }
}

InetAddress::InetAddress(const std::string& addr, int port)
    : port_(port),
      family_(FAMILY_IPv4),
      host_(addr) {

    if (!checkPort(port)) {
        port_ = 0;
        family_ = FAMILY_NONE;
        host_.clear();
        return;
    }

    if (!addr.empty() && addr.find(':') != addr.npos) {
        family_ = FAMILY_IPv6;
    }
}

InetAddress::InetAddress(const std::string& hostAndPort)
    : port_(0),
      family_(FAMILY_NONE) {
    if (hostAndPort.empty()) {
        LOG_WARN << "hostAndPort should not be empty.";
        return;
    }

    std::string host;
    std::string port;
    std::string::const_iterator it  = hostAndPort.begin();
    std::string::const_iterator end = hostAndPort.end();

    if (*it == '[') {
        ++it;

        while (it != end && *it != ']') { host += *it++; }

        if (it == end) {
            LOG_WARN << "Malformed IPv6 address: " << hostAndPort;
            return;
        }

        ++it;

        family_ = FAMILY_IPv6;
    }
    else {
        while (it != end && *it != ':') {
            host += *it++;
        }

        family_ = FAMILY_IPv4;
    }

    if (it != end && *it == ':') {
        ++it;

        while (it != end) {
            port += *it++;
        }
    }
    else {
        LOG_ERROR << "Missing port number " << hostAndPort;
        family_ = FAMILY_NONE;
        return;
    }

    if (port[0] >= '0' && port[0] <= '9') {
        int p = StringUtil::strto32(port);

        if (checkPort(p)) {
            port_ = p;
            host_ = host;
        }
        else {
            family_ = FAMILY_NONE;
        }
    }
    else {
        LOG_ERROR << "InetAddress does not support service ("
                  << port << ") as port";

        family_ = FAMILY_NONE;
    }
}

InetAddress::InetAddress(const InetAddress& addr)
    : port_(addr.port_),
      family_(addr.family_),
      host_(addr.host_) {
}

InetAddress& InetAddress::operator = (const InetAddress& addr) {
    if (&addr != this) {
        port_ = addr.port_;
        family_ = addr.family_;
        host_ = addr.host_;
    }

    return *this;
}

bool InetAddress::operator==(const InetAddress& addr) const {
    if (&addr == this) {
        return true;
    }

    return port_ == addr.port_ &&
           family_ == addr.family_ &&
           host_ == addr.host_;
}

bool InetAddress::operator!=(const InetAddress& addr) const {
    return !((*this) == addr);
}

void InetAddress::swap(InetAddress& addr) {
    InetAddress tmp(*this);
    *this = addr;
    addr = tmp;
}

const std::string& InetAddress::host() const {
    if (!host_.empty()) {
        return host_;
    }
    else if (family_ == FAMILY_IPv4) {
        return ANY_IPV4;
    }
    else if (family_ == FAMILY_IPv6) {
        return ANY_IPV6;
    }
    else {
        return host_;
    }
}

std::string InetAddress::toString() const {
    std::string buf = host();

    if (port_ > 0) {
        StringUtil::printf(&buf, ":%d", port_);
    }
    else {
        buf = "EMPTY_ADDRESS";
    }

    return buf;
}

bool InetAddress::checkFamily(int family) {
    if (family == FAMILY_IPv4 || family == FAMILY_IPv6) {
        return true;
    }
    else {
        LOG_ERROR << "family: " << family << " is illegal";
        return false;
    }
}

bool InetAddress::checkPort(int port) {
    if (port > 0 && port < 0xFFFF) {
        return true;
    }
    else {
        LOG_ERROR << "port : " << port << " is illegal";
        return false;
    }
}

}
}
