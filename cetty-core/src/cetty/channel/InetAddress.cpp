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

const InetAddress InetAddress::NULL_ADDRESS;

static const std::string ANY_IPV4 = "0.0.0.0";
static const std::string ANY_IPV6 = "0.0.0.0";

InetAddress::InetAddress()
    : port_(0),
      family_(FAMILY_NONE) {
}

InetAddress::InetAddress(int family)
    : port_(0),
      family_(family) {
}

InetAddress::InetAddress(int family, uint16_t port)
    : port_(port),
      family_(family) {
}

InetAddress::InetAddress(const std::string& addr, uint16_t port)
    : port_(port),
      family_(FAMILY_IPv4),
      host_(addr) {
}

InetAddress::InetAddress(const std::string& addr, const std::string& port)
    : host_(addr),
      family_(FAMILY_IPv4),
      service_(port) {
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
        LOG_WARN << "Missing port number " << hostAndPort;
        family_ = FAMILY_NONE;
        return;
    }

    if (port[0] >= '0' && port[0] <= '9') {
        int p = StringUtil::strto32(port);

        if (p > 0 && p < 0xFFFF) {
            port_ = p;
            host_ = host;
        }
        else {
            LOG_WARN << "port number " << p << " > 65536";
            family_ = FAMILY_NONE;
        }

        return;
    }

    service_ = port;
    host_ = host;
}

InetAddress::InetAddress(const InetAddress& addr)
    : port_(addr.port_),
      family_(addr.family_),
      host_(addr.host_),
      service_(addr.service_) {
}

InetAddress& InetAddress::operator = (const InetAddress& addr) {
    if (&addr != this) {
        port_ = addr.port_;
        family_ = addr.family_;

        host_ = addr.host_;
        service_ = addr.service_;
    }

    return *this;
}

bool InetAddress::operator==(const InetAddress& addr) const {
    if (&addr == this) {
        return true;
    }

    return port_ == addr.port_ &&
           family_ == addr.family_ &&
           host_ == addr.host_ &&
           service_ == addr.service_;
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
    else if (!service_.empty()){
        StringUtil::printf(&buf, ":%s", service_.c_str());
    }
    else {
        buf = "INVALID_ADDRESS";
    }
    
    return buf;
}

}
}
