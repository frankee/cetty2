#if !defined(CETTY_CHANNEL_CHANNELINITIALIZER_H)
#define CETTY_CHANNEL_CHANNELINITIALIZER_H

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
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerWrapper.h>
#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace channel {

template<typename T>
class ChannelInitializer {
public:
    typedef typename ChannelHandlerWrapper<T>::Handler Handler;
    typedef typename ChannelHandlerWrapper<T>::HandlerPtr HandlerPtr;
    typedef typename ChannelHandlerWrapper<T>::Handler::Context Context;

public:
    ChannelInitializer()
        : name_("auto1") {
    }

    ChannelInitializer(const std::string& name)
        : name_(name) {
    }
    
    bool operator()(const ChannelPtr& channel) {
        ChannelPipeline& pipeline = channel->pipeline();
        pipeline.addLast<HandlerPtr>(name, HandlerPtr(new Handler));
        return true;
    }

private:
    std::string name_;
};

template<typename T1, typename T2>
class ChannelInitializer {
public:
    typedef typename ChannelHandlerWrapper<T1>::Handler Handler1;
    typedef typename ChannelHandlerWrapper<T1>::HandlerPtr Handler1Ptr;
    typedef typename ChannelHandlerWrapper<T1>::Handler::Context Context1;

    typedef typename ChannelHandlerWrapper<T2>::Handler Handler2;
    typedef typename ChannelHandlerWrapper<T2>::HandlerPtr Handler2Ptr;
    typedef typename ChannelHandlerWrapper<T2>::Handler::Context Context2;

public:
    ChannelInitializer() {}

    bool operator()(const ChannelPtr& channel) {
        ChannelPipeline& pipeline = channel->pipeline();
        pipeline.addLast<Handler1Ptr>("auto1", Handler1Ptr(new Handler1));
        pipeline.addLast<Handler2Ptr>("auto2", Handler2Ptr(new Handler2));
        return true;
    }
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELINITIALIZER_H)

// Local Variables:
// mode: c++
// End:
