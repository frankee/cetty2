#if !defined(CETTY_REDIS_REDISMESSAGEHANDLER_H)
#define CETTY_REDIS_REDISMESSAGEHANDLER_H

/**
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

#include <deque>
#include <boost/function.hpp>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/SimpleChannelUpstreamHandler.h>
#include "RedisReplyMessage.h"

namespace cetty { namespace redis {

using namespace cetty::channel;

class RedisMessageHandler : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    typedef boost::function1<void, const RedisReplyMessagePtr&> CallBack;
    typedef boost::function0<void> DisConnectCallback;

public:
    RedisMessageHandler(const DisConnectCallback& callback) : disconnectCallback(callback) {}
    RedisMessageHandler() {}
    virtual ~RedisMessageHandler() {}

    virtual ChannelHandlerPtr clone() {
        RedisMessageHandler* hander = new RedisMessageHandler(disconnectCallback);
        //hander->appendCallback(callbacks);
        return ChannelHandlerPtr(hander);
    }

    virtual std::string toString() const {
        return std::string("RedisMessageHandler");
    }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        const RedisReplyMessagePtr& msg = e.getMessage().smartPointer<RedisReplyMessage>();
        if (callbacks.empty()) {
            printf("Received Message, but no callbacks.\n");
        }
        else {
            try {
                const CallBack& callback = callbacks.front();
                if (callback) {
                    callback(msg);
                }
                else {
                    printf("CallBack empty, SOMETHING MAY BE ERROR!\n");
                }
                
                callbacks.pop_front();
            }
            catch (const std::exception& e) {
            	printf("%s.\n", e.what());
            }
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        printf("Received an exception.\n");
    }

    virtual void channelDisconnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        if (disconnectCallback) {
            disconnectCallback();
        }
    }

    void appendCallback(const CallBack& callback) {
        this->callbacks.push_back(callback);
    }

private:
    std::deque<CallBack> callbacks;
    DisConnectCallback disconnectCallback;
};

typedef boost::intrusive_ptr<RedisMessageHandler> RedisMessageHandlerPtr;

}}

#endif //#if !defined(CETTY_REDIS_REDISMESSAGEHANDLER_H)