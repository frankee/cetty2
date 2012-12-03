#if !defined(CETTY_SERVICE_CLIENTSERVICE_H)
#define CETTY_SERVICE_CLIENTSERVICE_H

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

#include <vector>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelConfig.h>

#include <cetty/service/ServiceFuture.h>
#include <cetty/service/ClientServicePtr.h>
//#include <cetty/service/ServiceRequestHandler.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

class ClientService : public cetty::channel::Channel {
public:
    ClientService(const EventLoopPtr& eventLoop);

    virtual ~ClientService() {}

    virtual bool isOpen() const;
    virtual bool isActive() const;

protected:
    virtual void doBind(const SocketAddress& localAddress);
    virtual void doDisconnect();
    virtual void doClose();

    template<typename ReqT, typename RepT>
    void writeRequest(const ReqT& request,
        const boost::intrusive_ptr<ServiceFuture<RepT> >& future) {
            boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > outstanding(
                new OutstandingCall<ReqT, RepT>(request, future));
            channel->writeMessage(outstanding);
    }

protected:
    EventLoopPtr eventLoop_;
};

template<typename ReqT, typename RepT>
void callMethod(const ChannelPtr& channel,
                const ReqT& request,
                const boost::intrusive_ptr<ServiceFuture<RepT> >& future) {
    if (channel) {
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > outstanding(
            new OutstandingCall<ReqT, RepT>(request, future));
        channel->writeMessage(outstanding);
    }
}

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTSERVICE_H)

// Local Variables:
// mode: c++
// End:
