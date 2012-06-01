#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOIPADDRESSIMPLFACTORY_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOIPADDRESSIMPLFACTORY_H

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

#include <cetty/channel/IpAddressImpl.h>
#include <cetty/channel/socket/asio/AsioIpAddressImpl.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

class AsioIpAddressImplFactory : public ::cetty::channel::IpAddressImplFactory {
public:
    AsioIpAddressImplFactory() {}
    virtual ~AsioIpAddressImplFactory() {}

    virtual IpAddressImpl* create(int family) {
        return new AsioIpAddressImpl(family);
    }
    virtual IpAddressImpl* create(const std::string& addr) {
        return new AsioIpAddressImpl(addr);
    }
    virtual IpAddressImpl* create(const std::string& addr,int family) {
        return new AsioIpAddressImpl(addr, family);
    }
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOIPADDRESSIMPLFACTORY_H)
