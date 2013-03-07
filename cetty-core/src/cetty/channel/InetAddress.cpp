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

namespace cetty {
namespace channel {

using namespace cetty::util;

const InetAddress InetAddress::NULL_ADDRESS;

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
      host_(addr) {
}

InetAddress::InetAddress(const std::string& addr, const std::string& port)
    : host_(addr),
      service_(port) {
}

InetAddress::InetAddress(const std::string& hostAndPort) {
    BOOST_ASSERT(!hostAndPort.empty());

    if (hostAndPort.empty()) {
        throw InvalidArgumentException("has no host and port information.");
    }

    std::string host;
    std::string port;
    std::string::const_iterator it  = hostAndPort.begin();
    std::string::const_iterator end = hostAndPort.end();

    if (*it == '[') {
        ++it;

        while (it != end && *it != ']') { host += *it++; }

        if (it == end) { throw InvalidArgumentException("Malformed IPv6 address"); }

        ++it;
    }
    else {
        while (it != end && *it != ':') { host += *it++; }
    }

    if (it != end && *it == ':') {
        ++it;

        while (it != end) { port += *it++; }
    }
    else {
        throw InvalidArgumentException("Missing port number");
    }

    if (port[0] >= '0' && port[0] <= '9') {
        int p = StringUtil::strto32(port);

        if (p > 0 && p < 0xFFFF) {
            //init(host, p);
            return;
        }
    }

    //init(host, port);
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
}

std::string InetAddress::toString() const {
    std::string buf = host_;

    if (host_.empty()) {
        buf = "0.0.0.0";
    }
    
    if (port_ > 0) {
        StringUtil::printf(&buf, ":%d", port_);
    }
    else if (!service_.empty()){
        StringUtil::printf(&buf, ":%s", service_.c_str());
    }
    else {
        buf = "NULL_ADDRESS";
    }
    
    return buf;
}

}
}
