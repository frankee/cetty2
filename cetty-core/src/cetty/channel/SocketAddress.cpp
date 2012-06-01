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

#include <boost/assert.hpp>

#include <cetty/channel/IpAddress.h>
#include <cetty/channel/SocketAddressImpl.h>
#include <cetty/channel/SocketAddress.h>

#include <cetty/util/Exception.h>
#include <cetty/util/Integer.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

const SocketAddress SocketAddress::NULL_ADDRESS;
SocketAddressImplFactory* SocketAddress::factory = NULL;

SocketAddress::SocketAddress() {
}

SocketAddress::SocketAddress(int family) {
    BOOST_ASSERT(factory);

    if (factory) {
        impl = SmartPointer(factory->create(family));
    }
}

SocketAddress::SocketAddress(int family, int port) {
    BOOST_ASSERT(factory);

    if (factory) {
        impl = SmartPointer(factory->create(family, port));
    }
}

SocketAddress::SocketAddress(const IpAddress& addr, int port) {
    BOOST_ASSERT(factory);

    if (factory) {
        impl = SmartPointer(factory->create(addr, port));
    }
}

SocketAddress::SocketAddress(const std::string& addr, int port) {
    init(addr, port);
}

SocketAddress::SocketAddress(const std::string& addr, const std::string& port) {
    init(addr, port);
}

SocketAddress::SocketAddress(const std::string& hostAndPort) {
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
        int p;
        Integer::tryParse(port, p);

        if (p > 0 && p < 0xFFFF) {
            init(host, p);
            return;
        }
    }

    init(host, port);
}

SocketAddress::SocketAddress(const SocketAddress& addr) : impl(addr.impl) {
}

SocketAddress::SocketAddress(const SmartPointer& impl) : impl(impl) {
}

SocketAddress::~SocketAddress() {
}

SocketAddress& SocketAddress::operator = (const SocketAddress& addr) {
    if (&addr != this) {
        impl = addr.impl;
    }

    return *this;
}

bool SocketAddress::operator==(const SocketAddress& addr) const {
    if (&addr == this) { return true; }

    if (impl) {
        if (addr.impl) {
            return impl->equals(*addr.impl);
        }
        else {
            return false;
        }
    }
    else {
        if (addr.impl) {
            return false;
        }
        else {
            return true;
        }
    }
}

bool SocketAddress::operator!=(const SocketAddress& addr) const {
    return !((*this) == addr);
}

void SocketAddress::swap(SocketAddress& addr) {
    impl.swap(addr.impl);
}

const IpAddress& SocketAddress::ipAddress() const {
    if (impl) {
        return impl->ipAddress();
    }

    return IpAddress::NULL_ADDRESS;
}

std::string SocketAddress::address() const {
    if (impl) {
        return impl->address();
    }

    return std::string("");
}

std::string SocketAddress::hostName() const {
    if (impl) {
        return impl->hostName();
    }

    return "";
}

int SocketAddress::port() const {
    if (impl) {
        return impl->port();
    }

    return 0;
}

std::string SocketAddress::toString() const {
    if (impl) {
        return impl->toString();
    }

    return "NULL_ADDRESS";
}

void SocketAddress::init(const std::string& host, int port) {
    BOOST_ASSERT(factory);

    if (host.empty() || port <= 0 || port >= 0xFFFF) {
        throw InvalidArgumentException("host or port is invalid.");
    }

    if (factory) {
        impl = SmartPointer(factory->create(host, port));
    }
}

void SocketAddress::init(const std::string& host, const std::string& port) {
    if (port.empty()) {
        throw InvalidArgumentException("port is invalid.");
    }

    if (factory) {
        impl = SmartPointer(factory->create(host, port));
    }
}

int SocketAddress::family() const {
    if (impl) {
        return impl->ipAddress().family();
    }

    return IpAddress::IPv4;
}

void SocketAddress::setFacotry(SocketAddressImplFactory* factory) {
    if (factory) {
        if (NULL == SocketAddress::factory) {
            SocketAddress::factory = factory;
        }
        else {
            //logger here.
        }
    }
}

void SocketAddress::resetFactory() {
    //log here.
    SocketAddress::factory = NULL;
}

bool SocketAddress::hasFactory() {
    return NULL != SocketAddress::factory;
}



}
}
