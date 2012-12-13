#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHEADLINKCONTEXT_H)
#define CETTY_CHANNEL_CHANNELMESSAGEHEADLINKCONTEXT_H

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
namespace cetty {
namespace channel {

template<typename H,
         typename OutboundIn,
         typename OutboundInContainer>
class ChannelMessageHeadLinkContext : public ChannelHandlerContext {
public:
    typedef ChannelMessageHeadLinkContext<H,
            OutboundIn,
            OutboundInContainer> Context;

    typedef typename ChannelHandlerWrapper<H>::Handler Handler;
    typedef typename ChannelHandlerWrapper<H>::HandlerPtr HandlerPtr;
    typedef typename ChannelHandlerWrapper<H>::StoredHandlerPtr StoredHandlerPtr;

    typedef boost::function<void (HandlerPtr const&, Context&)> RegisterCallback;

public:
    ChannelMessageHeadLinkContext(const ChannelPtr& channel,
                                  const std::string& name,
                                  const HandlerPtr& handler)
        : ChannelHandlerContext(name),
          handler_(handler),
          inboundTransfer_(*this),
          outboundTransfer_(*this) {
        if (handler) {
            handler->registerTo(*this);
        }
    }

    ChannelMessageHeadLinkContext(const std::string& name,
                                  const HandlerPtr& handler,
                                  const RegisterCallback& registerCallback)
        : ChannelHandlerContext(name),
          channel_(),
          handler_(handler) {
        if (handler && registerCallback) {
            registerCallback(handler, *this);
        }
    }

    ChannelMessageHeadLinkContext(const std::string& name,
                                  const EventLoopPtr& eventLoop,
                                  const HandlerPtr& handler)
        : ChannelHandlerContext(name, eventLoop) {
        if (handler) {
            handler->registerTo(*this);
        }
    }

    ChannelMessageHeadLinkContext(const std::string& name,
                                  const HandlerPtr& handler,
                                  const EventLoopPtr& eventLoop,
                                  const RegisterCallback& registerCallback)
        : ChannelHandlerContext(name, eventLoop),
          handler_(handler) {
        if (handler && registerCallback) {
            registerCallback(handler, *this);
        }
    }

    virtual boost::any getInboundMessageContainer() {
        return boost::any(&inboundContainer_);
    }

    virtual boost::any getOutboundMessageContainer() {
        return boost::any();
    }

    virtual void initialize(ChannelPipeline* pipeline) {
        ChannelHandlerContext::initialize(pipeline);
        inboundContainer_.setEventLoop(eventLoop());
        outboundContainer_.setEventLoop(eventLoop());
    }

    virtual void onPipelineChanged() {
        ChannelHandlerContext::onPipelineChanged();
    }

private:
    ChannelWeakPtr channel_;
    StoredHandlerPtr handler_;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHEADLINKCONTEXT_H)

// Local Variables:
// mode: c++
// End:
