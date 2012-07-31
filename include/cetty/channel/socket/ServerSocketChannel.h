#if !defined(CETTY_CHANNEL_SOCKET_SERVERSOCKETCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_SERVERSOCKETCHANNEL_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/AbstractServerChannel.h>

namespace cetty {
namespace channel  {
namespace socket {

/**
 * A TCP/IP {@link ServerChannel} which accepts incoming TCP/IP connections.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.composedOf org.jboss.netty.channel.socket.ServerSocketChannelConfig
 */

class ServerSocketChannel : public cetty::channel::AbstractServerChannel {
protected:
    /**
     * Creates a new instance.
     *
     * @param factory
     *        the factory which created this channel
     * @param pipeline
     *        the pipeline which is going to be attached to this channel
     * @param sink
     *        the sink which will receive downstream events from the pipeline
     *        and send upstream events to the pipeline
     */
    ServerSocketChannel(
        const EventLoopPtr& eventLoop,
        const ChannelFactoryPtr& factory,
        const ChannelPipelinePtr& pipeline) 
        : AbstractServerChannel(eventLoop, factory, pipeline) {
    }

    virtual ~ServerSocketChannel() {}
};

}
}
}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_SERVERSOCKETCHANNEL_H)

// Local Variables:
// mode: c++
// End:
