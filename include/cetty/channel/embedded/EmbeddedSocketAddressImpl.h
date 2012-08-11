#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDSOCKETADDRESSIMPL_H)
#define CETTY_CHANNEL_EMBEDDED_EMBEDDEDSOCKETADDRESSIMPL_H
/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
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

#include <cetty/channel/SocketAddressImpl.h>
#include <cetty/channel/IpAddress.h>

namespace cetty {
namespace channel {
namespace embedded {

using namespace cetty::channel;

class EmbeddedSocketAddressImpl : public cetty::channel::SocketAddressImpl {
public:
    EmbeddedSocketAddressImpl() {}
    virtual ~EmbeddedSocketAddressImpl() {}

    virtual const IpAddress& ipAddress() const {
        return IpAddress::NULL_ADDRESS;
    }
    virtual int port() const { return 0; }

    virtual std::string address() const { return "embed"; }

    virtual std::string hostName() const { return "embed"; }

    virtual bool equals(const SocketAddressImpl& addr) const {
        return address() == addr.address()
               && hostName() == addr.hostName()
               && port() == addr.port()
               && ipAddress() == addr.ipAddress();
    }

    virtual std::string toString() const {
        return "EmbeddedSocketAddress";
    }
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDSOCKETADDRESSIMPL_H)

// Local Variables:
// mode: c++
// End:
