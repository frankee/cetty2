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

#include <cetty/channel/asio/AsioSocketAddressImplFactory.h>
#include <cetty/channel/asio/AsioSocketAddressImpl.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;

SocketAddressImpl* AsioTcpSocketAddressImplFactory::create(int family) {
    return new AsioSocketAddressImpl<boost::asio::ip::tcp>(ioService, family);
}

SocketAddressImpl* AsioTcpSocketAddressImplFactory::create(int family, int port) {
    return new AsioSocketAddressImpl<boost::asio::ip::tcp>(ioService, family, port);
}

SocketAddressImpl* AsioTcpSocketAddressImplFactory::create(const IpAddress& addr, int port) {
    return new AsioSocketAddressImpl<boost::asio::ip::tcp>(ioService, addr, port);
}

SocketAddressImpl* AsioTcpSocketAddressImplFactory::create(const std::string& addr, int port) {
    return new AsioSocketAddressImpl<boost::asio::ip::tcp>(ioService, addr, port);
}

SocketAddressImpl* AsioTcpSocketAddressImplFactory::create(const std::string& addr, const std::string& service) {
    return new AsioSocketAddressImpl<boost::asio::ip::tcp>(ioService, addr, service);
}

SocketAddressImpl* AsioUdpSocketAddressImplFactory::create(int family) {
    return new AsioSocketAddressImpl<boost::asio::ip::udp>(ioService, family);
}

SocketAddressImpl* AsioUdpSocketAddressImplFactory::create(int family, int port) {
    return new AsioSocketAddressImpl<boost::asio::ip::udp>(ioService, family, port);
}

SocketAddressImpl* AsioUdpSocketAddressImplFactory::create(const IpAddress& addr, int port) {
    return new AsioSocketAddressImpl<boost::asio::ip::udp>(ioService, addr, port);
}

SocketAddressImpl* AsioUdpSocketAddressImplFactory::create(const std::string& addr, int port) {
    return new AsioSocketAddressImpl<boost::asio::ip::udp>(ioService, addr, port);
}

SocketAddressImpl* AsioUdpSocketAddressImplFactory::create(const std::string& addr, const std::string& service) {
    return new AsioSocketAddressImpl<boost::asio::ip::udp>(ioService, addr, service);
}

}
}
}
}
