#if !defined(CETTY_CHANNEL_SOCKETADDRESSIMPL_H)
#define CETTY_CHANNEL_SOCKETADDRESSIMPL_H

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

#include <cetty/channel/IpAddress.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace channel {

class SocketAddressImpl : public cetty::util::ReferenceCounter<SocketAddressImpl> {
public:
    virtual ~SocketAddressImpl()  {}

    virtual const IpAddress& ipAddress() const = 0;
    virtual int port() const = 0;

    virtual std::string address() const = 0;

    virtual std::string hostName() const = 0;

    virtual bool equals(const SocketAddressImpl& addr) const = 0;
    virtual std::string toString() const = 0;

protected:
    SocketAddressImpl() {}

private:
    SocketAddressImpl(const SocketAddressImpl&);
    SocketAddressImpl& operator = (const SocketAddressImpl&);
};

class SocketAddressImplFactory {
public:
    virtual ~SocketAddressImplFactory() {}

    virtual SocketAddressImpl* create(int family) = 0;
    virtual SocketAddressImpl* create(int family, int port) = 0;
    virtual SocketAddressImpl* create(const IpAddress& addr, int port) = 0;
    virtual SocketAddressImpl* create(const std::string& addr, int port) = 0;
    virtual SocketAddressImpl* create(const std::string& addr, const std::string& service) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKETADDRESSIMPL_H)

// Local Variables:
// mode: c++
// End:

