#if !defined(CETTY_SERVICE_POOL_CONNECTIONPOOL_H)
#define CETTY_SERVICE_POOL_CONNECTIONPOOL_H

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

#include <map>
#include <deque>
#include <vector>

#include <boost/function.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/service/Connection.h>
#include <cetty/bootstrap/ClientBootstrap.h>

namespace cetty {
namespace service {
namespace pool {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::bootstrap;

class ConnectionPool : private boost::noncopyable {
public:
    typedef boost::function1<void, const ChannelPtr&> ConnectedCallback;

public:
    ConnectionPool(const Connections& connections);
    ConnectionPool(const Connections& connections,
                   const EventLoopPtr& eventLoop);

    virtual ~ConnectionPool();

public:
    ChannelPtr getChannel();
    ChannelPtr getChannel(const ConnectedCallback& callback);

    ClientBootstrap& bootstrap() {
        return bootstrap_;
    }

    void setChannelInitializer(const Channel::Initializer& initializer);

    void close() {}

    //protected:
    void connectedCallback(const ChannelFuture& future);

protected:
    struct ChannelConnection {
        std::string host;
        ChannelPtr channel;
        int queuedRequest;
        boost::posix_time::ptime start;
        boost::posix_time::ptime lastActive;
    };

protected:
    std::vector<Connection> connections_;
    ClientBootstrap bootstrap_;

private:
    bool connecting_;
    std::deque<ConnectedCallback> callbacks_;

    boost::ptr_map<int, ChannelConnection> channels_;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_POOL_CONNECTIONPOOL_H)

// Local Variables:
// mode: c++
// End:
