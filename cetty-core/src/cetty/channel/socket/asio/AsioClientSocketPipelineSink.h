#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETPIPELINESINK_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETPIPELINESINK_H

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

#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/ChannelFutureFwd.h>
#include <cetty/channel/AbstractChannelSink.h>
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>

namespace cetty {
    namespace channel {
        class SocketAddress;
    }
}

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::logging;

/**
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */
class AsioClientSocketPipelineSink : public cetty::channel::AbstractChannelSink {
public:
    AsioClientSocketPipelineSink(bool needStartRunService);

    virtual ~AsioClientSocketPipelineSink();

    virtual void writeRequested(const ChannelPipeline& pipeline,
                                 const MessageEvent& e);

    virtual void stateChangeRequested(const ChannelPipeline& pipeline,
                                      const ChannelStateEvent& e);

private:
    void connect(const ChannelPtr& channel,
        const ChannelFuturePtr& cf,
        const SocketAddress& remoteAddress);

    void handleStateChange(const ChannelStateEvent& evt);

private:
    static InternalLogger* logger;

private:
    /// only manually start the service when has one service and in the main thread.
    bool needStartRunService;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOCLIENTSOCKETPIPELINESINK_H)
