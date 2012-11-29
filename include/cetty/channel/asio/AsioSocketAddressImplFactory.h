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

#include <cetty/channel/SocketAddressImpl.h>

namespace boost {
namespace asio {
class io_service;
}
}

namespace cetty {
namespace channel {
namespace asio {

class AsioTcpSocketAddressImplFactory : public ::cetty::channel::SocketAddressImplFactory {
public:
    AsioTcpSocketAddressImplFactory(boost::asio::io_service& ioService)
        : ioService(ioService) {}

    virtual ~AsioTcpSocketAddressImplFactory() {}

    virtual SocketAddressImpl* create(int family);
    virtual SocketAddressImpl* create(int family, int port);
    virtual SocketAddressImpl* create(const IpAddress& addr, int port);
    virtual SocketAddressImpl* create(const std::string& addr, int port);
    virtual SocketAddressImpl* create(const std::string& addr, const std::string& service);

private:
    boost::asio::io_service& ioService;
};

class AsioUdpSocketAddressImplFactory : public ::cetty::channel::SocketAddressImplFactory {
public:
    AsioUdpSocketAddressImplFactory(boost::asio::io_service& ioService)
        : ioService(ioService) {}

    virtual ~AsioUdpSocketAddressImplFactory() {}

    virtual SocketAddressImpl* create(int family);
    virtual SocketAddressImpl* create(int family, int port);
    virtual SocketAddressImpl* create(const IpAddress& addr, int port);
    virtual SocketAddressImpl* create(const std::string& addr, int port);
    virtual SocketAddressImpl* create(const std::string& addr, const std::string& service);

private:
    boost::asio::io_service& ioService;
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETADDRESSIMPLFACTORY_H)

// Local Variables:
// mode: c++
// End:
