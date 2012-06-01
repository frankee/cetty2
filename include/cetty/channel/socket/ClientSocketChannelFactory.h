#if !defined(CETTY_CHANNEL_SOCKET_CLIENTSOCKETCHANNELFACTORY_H)
#define CETTY_CHANNEL_SOCKET_CLIENTSOCKETCHANNELFACTORY_H

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

#include <cetty/channel/socket/SocketChannelFactory.h>

namespace cetty {
namespace channel {
namespace socket {

/**
 * A {@link ChannelFactory} which creates a client-side {@link SocketChannel}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.has org.jboss.netty.channel.socket.SocketChannel oneway - - creates
 */
class ClientSocketChannelFactory : public SocketChannelFactory {
public:
    virtual ~ClientSocketChannelFactory() {}
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_CLIENTSOCKETCHANNELFACTORY_H)

// Local Variables:
// mode: c++
// End:

