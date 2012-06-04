#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DUMMYHANDLER_H)
#define CETTY_CHANNEL_SOCKET_ASIO_DUMMYHANDLER_H

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

#include "cetty/channel/SimpleChannelUpstreamHandler.h"

using namespace cetty::channel;

class DummyHandler : public SimpleChannelUpstreamHandler {
public:
    volatile bool connected;
    volatile bool closed;

    DummyHandler() {}
    virtual ~DummyHandler() {}

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new DummyHandler);
    }

    virtual void channelConnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        connected = true;
    }

    virtual void channelClosed(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        closed = true;
    }
};

typedef boost::shared_ptr<DummyHandler> DummyHandlerPtr;

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_DUMMYHANDLER_H)
