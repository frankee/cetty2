#if !defined(CETTY_CHANNEL_INETADDRESS_H)
#define CETTY_CHANNEL_INETADDRESS_H

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
#include <cetty/Types.h>

namespace cetty {
namespace channel {

/**
 * This class represents an internet (IP) endpoint/socket
 * address. The address can belong either to the
 * IPv4 or the IPv6 address family and consists of a
 * host address and a port number.
 */
class InetAddress {
public:
    /**
     * Possible address families for IP addresses.
     */
    enum Family {
        FAMILY_NONE = 0,
        FAMILY_IPv4,
        FAMILY_IPv6
    };

    static const InetAddress NULL_ADDRESS;

public:
    InetAddress();

    /**
     * Creates a wildcard (all zero) IPv4 InetAddress.
     */
    InetAddress(int family);


    InetAddress(int family, uint16_t port);

    /**
     * Creates a InetAddress from an IP address and a port number.
     *
     * The IP address must either be a domain name, or it must
     * be in dotted decimal (IPv4) or hex string (IPv6) format.
     */
    InetAddress(const std::string& host, uint16_t port);

    /**
     * Creates a InetAddress from an IP address and a
     * service name or port number.
     *
     * The IP address must either be a domain name, or it must
     * be in dotted decimal (IPv4) or hex string (IPv6) format.
     *
     * The given port must either be a decimal port number, or
     * a service name.
     */
    InetAddress(const std::string& host, const std::string& port);

    /**
     * Creates a InetAddress from an IP address or host name and a
     * port number/service name. Host name/address and port number must
     * be separated by a colon. In case of an IPv6 address,
     * the address part must be enclosed in brackets.
     *
     * Examples:
     *     192.168.1.10:80
     *     [::FFFF:192.168.1.120]:2040
     *     www.appinf.com:8080
     */
    InetAddress(const std::string& hostAndPort);

    /**
     * Creates a InetAddress by copying another one.
     */
    InetAddress(const InetAddress& addr);

    /**
     * Assigns another InetAddress.
     */
    InetAddress& operator=(const InetAddress& addr);

    //bool operator!() const;

    operator bool() const {
        return !empty();
    }

    bool operator ==(const InetAddress& addr) const;
    bool operator !=(const InetAddress& addr) const;

    /**
     * Swaps the InetAddress with another one.
     */
    void swap(InetAddress& addr);

    /**
     * Returns the host IP address.
     */
    const std::string& host() const;

    /**
     * Returns the port number.
     */
    int port() const;

    /**
     * Returns the address family of the host's address.
     */
    int family() const;

    /**
     * return the InetAddress is not a NULL_ADDRESS.
     */
    bool empty() const {
        return host_.empty() && service_.empty() && port_ == 0 && family_ == FAMILY_NONE;
    }

    /**
     * Returns a string representation of the address.
     */
    std::string toString() const;

private:
    int port_;
    int family_;
    std::string host_;
    std::string service_;
};

inline
void swap(InetAddress& a1, InetAddress& a2) {
    a1.swap(a2);
}

inline
const std::string& InetAddress::host() const {
    return host_;
}

inline
int InetAddress::port() const {
    return port_;
}

inline
int InetAddress::family() const {
    return family_;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_INETADDRESS_H)

// Local Variables:
// mode: c++
// End:
