#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETADDRESSIMPL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETADDRESSIMPL_H

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

#include <string>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>

#include <cetty/channel/SocketAddressImpl.h>
#include <cetty/channel/IpAddress.h>

#include <cetty/util/Integer.h>
#include <cetty/util/Character.h>

#include <cetty/channel/socket/asio/AsioIpAddressImpl.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::util;

template <typename InternetProtocol>
class AsioSocketAddressImpl : public ::cetty::channel::SocketAddressImpl {
public:
    typedef boost::asio::ip::basic_endpoint<InternetProtocol> EndpointType;

public:
    AsioSocketAddressImpl(boost::asio::io_service& ioService)
        : ioService(ioService) {}

    AsioSocketAddressImpl(boost::asio::io_service& ioService, int family)
        : ioService(ioService) {}

    AsioSocketAddressImpl(boost::asio::io_service& ioService, int family, int port)
        : ioService(ioService) {
        if (family == IpAddress::IPv4) {
            endpoint = EndpointType(boost::asio::ip::address_v4(), port);
        }
        else {
            endpoint = EndpointType(boost::asio::ip::address_v6(), port);
        }

        hostNameStr = endpoint.address().to_string();
    }

    AsioSocketAddressImpl(boost::asio::io_service& ioService,
                          const IpAddress& addr, int port) : ioService(ioService), hostNameStr(addr.toString()) {
        init(hostNameStr, port);
    }

    AsioSocketAddressImpl(boost::asio::io_service& ioService,
                          const std::string& addr, int port) : ioService(ioService), hostNameStr(addr) {
        init(addr, port);
    }

    AsioSocketAddressImpl(boost::asio::io_service& ioService,
                          const std::string& addr, const std::string& service) : ioService(ioService), hostNameStr(addr) {
        init(addr, service);
    }

    AsioSocketAddressImpl(boost::asio::io_service& ioService,
                          const EndpointType& endpoint)
        : ioService(ioService), endpoint(endpoint), hostNameStr(endpoint.address().to_string()) {}

    virtual ~AsioSocketAddressImpl() {}

    virtual const IpAddress& ipAddress() const {
        if (!hostAddress.validated()) {
            hostAddress = IpAddress(
                              IpAddress::SmartPointer(new AsioIpAddressImpl(endpoint.address())));
        }

        return hostAddress;
    }

    virtual std::string address() const {
        return endpoint.address().to_string();
    }

    virtual int port() const {
        return endpoint.port();
    }

    virtual std::string hostName() const {
        return hostNameStr;
    }

    virtual bool equals(const SocketAddressImpl& addr) const {
        const AsioSocketAddressImpl* imp = dynamic_cast<const AsioSocketAddressImpl*>(&addr);

        //TODO make comparable between two different impl.
        if (NULL == imp) { return false; }

        return endpoint == imp->endpoint;
    }

    virtual std::string toString() const {
        return endpoint.address().to_string();
    }

    EndpointType& getEndpoint() { return endpoint; }
    const EndpointType& getEndpoint() const { return endpoint; }

    void setEndpoint(const EndpointType& endpoint) {
        this->endpoint = endpoint;
        hostAddress = IpAddress::NULL_ADDRESS;
    }

protected:
    void init(const std::string& addr, int port) {
        if (Character::isHex(addr[0])) {
            endpoint = boost::asio::ip::basic_endpoint<InternetProtocol>(
                           boost::asio::ip::address::from_string(addr),
                           port);
        }
        else {
            init(addr, Integer::toString(port));
        }
    }

    void init(const std::string& addr, const std::string& service) {
        boost::asio::ip::basic_resolver<InternetProtocol> resolver(ioService);
        typename boost::asio::ip::basic_resolver<InternetProtocol>::query query(addr, service);
        typename boost::asio::ip::basic_resolver<InternetProtocol>::iterator iterator = resolver.resolve(query);
        endpoint = *iterator;
    }

private:
    boost::asio::io_service& ioService;
    EndpointType             endpoint;
    std::string              hostNameStr;

    mutable IpAddress        hostAddress;
};

typedef AsioSocketAddressImpl<boost::asio::ip::tcp> AsioTcpSocketAddressImpl;
typedef AsioSocketAddressImpl<boost::asio::ip::udp> AsioUdpSocketAddressImpl;

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETADDRESSIMPL_H)
