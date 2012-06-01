#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETADDRESSIMPLFACTORY_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETADDRESSIMPLFACTORY_H

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

#include <cetty/channel/socket/asio/AsioSocketAddressImpl.h>
#include <cetty/channel/SocketAddressImpl.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

template <typename InternetProtocol>
class AsioSocketAddressImplFactory : public ::cetty::channel::SocketAddressImplFactory {
public:
    AsioSocketAddressImplFactory(boost::asio::io_service& ioService)
        : ioService(ioService) {}

    virtual ~AsioSocketAddressImplFactory() {}

    virtual SocketAddressImpl* create(int family) {
        return new AsioSocketAddressImpl<InternetProtocol>(ioService, family);
    }
    virtual SocketAddressImpl* create(int family, int port) {
        return new AsioSocketAddressImpl<InternetProtocol>(ioService, family, port);
    }
    virtual SocketAddressImpl* create(const IpAddress& addr, int port) {
        return new AsioSocketAddressImpl<InternetProtocol>(ioService, addr, port);
    }
    virtual SocketAddressImpl* create(const std::string& addr, int port) {
        return new AsioSocketAddressImpl<InternetProtocol>(ioService, addr, port);
    }
    virtual SocketAddressImpl* create(const std::string& addr, const std::string& service) {
        return new AsioSocketAddressImpl<InternetProtocol>(ioService, addr, service);
    }

private:
    boost::asio::io_service& ioService;
};

class AsioTcpSocketAddressImplFactory : public AsioSocketAddressImplFactory<boost::asio::ip::tcp> {
public:
    AsioTcpSocketAddressImplFactory(boost::asio::io_service& ioService)
        : AsioSocketAddressImplFactory<boost::asio::ip::tcp>(ioService) {}
    virtual ~AsioTcpSocketAddressImplFactory() {}
};

class AsioUdpSocketAddressImplFactory : public AsioSocketAddressImplFactory<boost::asio::ip::udp> {
public:
    AsioUdpSocketAddressImplFactory(boost::asio::io_service& ioService)
        : AsioSocketAddressImplFactory<boost::asio::ip::udp>(ioService) {}
    virtual ~AsioUdpSocketAddressImplFactory() {}
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETADDRESSIMPLFACTORY_H)
