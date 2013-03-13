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
#include <cetty/service/ClientServiceDispatcher.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

template<typename Request, typename Response>
class ClientService : public cetty::channel::Channel {
public:
    typedef ClientService<Request, Response> Self;

    typedef ClientServiceDispatcher<Self*,
            Request,
            Response> ClientDispatcher;

    typedef typename ClientDispatcher::Context Context;
    typedef typename ClientDispatcher::Context::Handler Handler;
    typedef typename ClientDispatcher::Context::HandlerPtr HandlerPtr;

public:
    ClientService(const EventLoopPtr& eventLoop,
                  const Initializer& initializer,
                  const Connections& connections)
        : Channel(ChannelPtr(), eventLoop),
          eventLoop_(eventLoop),
          dispatcher_(eventLoop, connections, initializer) {
    }

    virtual ~ClientService() {}

    void registerTo(Context& ctx) {
        dispatcher_.registerTo(ctx);

        ctx.setChannelOpenCallback(boost::bind(
                                       &ClientService::onOpen,
                                       this,
                                       _1));
    }

private:
    bool doBind(const InetAddress& localAddress) { return true; }
    bool doDisconnect() { return true; }
    bool doClose() { return true; }

    void doInitialize() {
        pipeline().setHead(new Context("dispatcher", this));
    }

    void onOpen(ChannelHandlerContext& ctx) {
        setActived();
        ctx.fireChannelActive(ctx);
    }

private:
    EventLoopPtr eventLoop_;
    ClientDispatcher dispatcher_;
};

template<typename Request, typename Response>
void callMethod(const ChannelPtr& channel,
                const Request& request,
                const boost::intrusive_ptr<ServiceFuture<Response> >& future) {
    if (channel) {
        boost::intrusive_ptr<OutstandingCall<Request, Response> > outstanding(
            new OutstandingCall<Request, Response>(request, future));
        channel->writeMessage(outstanding);
    }
}

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTSERVICE_H)

// Local Variables:
// mode: c++
// End:
