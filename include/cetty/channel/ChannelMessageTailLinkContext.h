#if !defined(CETTY_CHANNEL_CHANNELMESSAGETAILLINKCONTEXT_H)
#define CETTY_CHANNEL_CHANNELMESSAGETAILLINKCONTEXT_H

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

#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace channel {

template<typename H,
         typename InboundIn,
         typename InboundInContainer>
class ChannelMessageTailLinkContext : public ChannelHandlerContext {
public:
    typedef ChannelMessageTailLinkContext<H,
            InboundIn,
            InboundInContainer> Context;

    typedef typename ChannelHandlerWrapper<H>::Handler Handler;
    typedef typename ChannelHandlerWrapper<H>::HandlerPtr HandlerPtr;
    typedef typename ChannelHandlerWrapper<H>::StoredHandlerPtr StoredHandlerPtr;

    typedef boost::function<void (HandlerPtr const&, Context&)> RegisterCallback;

    typedef InboundInContainer InboundContainer;

public:
    ChannelMessageTailLinkContext(const std::string& name,
                                  const ChannelPtr& channel,
                                  const HandlerPtr& handler)
        : ChannelHandlerContext(name),
          channel_(channel),
          pipeline_(&channel->pipeline()),
          handler_(handler),
          inboundContainer_() {
        if (handler) {
            handler->registerTo(*this);
        }
    }

    ChannelMessageTailLinkContext(const std::string& name,
                                  const ChannelWeakPtr& channel,
                                  const HandlerPtr& handler)
        : ChannelHandlerContext(name),
          channel_(channel),
          pipeline_(&channel.lock()->pipeline()),
          handler_(handler),
          inboundContainer_() {
        if (handler) {
            handler->registerTo(*this);
        }
    }

    ChannelMessageTailLinkContext(const std::string& name,
                                  const ChannelPtr& channel,
                                  const HandlerPtr& handler,
                                  const RegisterCallback& registerCallback)
        : ChannelHandlerContext(name),
          channel_(channel),
          pipeline_(&channel->pipeline()),
          handler_(handler),
          inboundContainer_() {
        if (handler && registerCallback) {
            registerCallback(handler, *this);
        }
    }

    ChannelMessageTailLinkContext(const std::string& name,
                                  const ChannelPtr& channel,
                                  const EventLoopPtr& eventLoop,
                                  const HandlerPtr& handler)
        : ChannelHandlerContext(name, eventLoop),
          channel_(channel),
          pipeline_(&channel->pipeline()),
          handler_(handler),
          inboundContainer_() {
        if (handler) {
            handler->registerTo(*this);
        }
    }

    ChannelMessageTailLinkContext(const std::string& name,
                                  const ChannelPtr& channel,
                                  const HandlerPtr& handler,
                                  const EventLoopPtr& eventLoop,
                                  const RegisterCallback& registerCallback)
        : ChannelHandlerContext(name, eventLoop),
          channel_(channel),
          pipeline_(&channel->pipeline()),
          handler_(handler),
          inboundContainer_() {
        if (handler && registerCallback) {
            registerCallback(handler, *this);
        }
    }

    virtual boost::any getInboundMessageContainer() {
        if (!channel_.expired()) {
            if (!inboundContainer_) {
                inboundContainer_ =
                    pipeline_->inboundMessageContainer<InboundContainer>();
            }

            return boost::any(inboundContainer_);
        }

        return boost::any();
    }

    virtual boost::any getOutboundMessageContainer() {
        return boost::any();
    }

private:
    ChannelWeakPtr channel_;
    ChannelPipeline* pipeline_;

    StoredHandlerPtr handler_;

    InboundContainer* inboundContainer_;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGETAILLINKCONTEXT_H)

// Local Variables:
// mode: c++
// End:
