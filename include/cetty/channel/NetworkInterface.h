#if !defined(CETTY_CHANNEL_NETWORKINTERFACE_H)
#define CETTY_CHANNEL_NETWORKINTERFACE_H

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
#include <vector>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace cetty {
namespace channel {

class IpAddress;
class NetworkInterfaceImpl;

/**
 * This class represents a network interface.
 *
 * NetworkInterface is used with MulticastSocket to specify
 * multicast interfaces for sending and receiving multicast
 * messages.
 */
class NetworkInterface {
public:
    static const NetworkInterface NULL_INTERFACE;

    typedef boost::shared_ptr<NetworkInterfaceImpl> SharedPtr;
    typedef std::vector<NetworkInterface> NetworkInterfaceList;

public:

    /**
     * Creates a NetworkInterface representing the
     * default interface.
     *
     * The name is empty, the IP address is the wildcard
     * address and the index is zero.
     */
    NetworkInterface();

    NetworkInterface(const NetworkInterface& interfc);

    ~NetworkInterface();

    NetworkInterface& operator = (const NetworkInterface& interfc);

    bool operator == (const NetworkInterface& interfc) const;

    /**
     * Swaps the NetworkInterface with another one.
     */
    void swap(NetworkInterface& other);

    /**
     * Returns the interface index.
     *
     * Only supported if IPv6 is available.
     * Returns -1 if IPv6 is not available.
     */
    int getIndex() const;

    /**
     * Returns the interface name.
     */
    const std::string& getName() const;

    /**
     * Returns the interface display name.
     *
     * On Windows platforms, this is currently the network adapter
     * name. This may change to the "friendly name" of the network
     * connection in a future version, however.
     *
     * On other platforms this is the same as name().
     */
    const std::string& getDisplayName() const;

    /**
     * Returns the IP address bound to the interface.
     */
    const IpAddress& getIpAddress() const;

    /**
     * Returns true if the interface supports IPv6.
     */
    bool isIPv6() const;

    /**
     * Returns the NetworkInterface for the given name.
     *
     * If requireIPv6 is false, an IPv4 interface is returned.
     * Otherwise, an IPv6 interface is returned.
     *
     * Throws an InterfaceNotFoundException if an interface
     * with the give name does not exist.
     */
    static NetworkInterface getByName(const std::string& name, bool requireIPv6 = false);

    /**
     * Returns the NetworkInterface for the given IP address.
     *
     * Throws an InterfaceNotFoundException if an interface
     * with the give address does not exist.
     */
    static NetworkInterface geByIpAddress(const IpAddress& address);


    /**
     * Returns the NetworkInterface for the given interface index.
     * If an index of 0 is specified, a NetworkInterface instance
     * representing the default interface (empty name and
     * wildcard address) is returned.
     *
     * Throws an InterfaceNotFoundException if an interface
     * with the given index does not exist (or IPv6 is not
     * available).
     */
    static NetworkInterface getByIndex(int index);

    /**
     * Returns all the interfaces on this machine.
     * Returns null if no network interfaces could be found on this machine.
     *
     * NOTE: can use getNetworkInterfaces()+getIpAddress() to obtain all IP addresses for this node
     *       If there are multiple addresses bound to one interface,
     *       multiple NetworkInterface instances are created for
     *       the same interface.
     * Returns:
     *       an Enumeration of NetworkInterfaces found on this machine
     * Throws:
     *       SocketException - if an I/O error occurs.
     */
    static NetworkInterfaceList getNetInterfaces();

protected:
    NetworkInterface(const std::string& name,const std::string& displayName, const IpAddress& address, int index = -1);
    NetworkInterface(const std::string& name, const std::string& displayName, const IpAddress& address, const IpAddress& subnetMask, const IpAddress& broadcastAddress, int index = -1);
    NetworkInterface(const std::string& name, const IpAddress& address, int index = -1);
    NetworkInterface(const std::string& name, const IpAddress& address, const IpAddress& subnetMask, const IpAddress& broadcastAddress, int index = -1);

private:
    SharedPtr impl;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_NETWORKINTERFACE_H)

// Local Variables:
// mode: c++
// End:

