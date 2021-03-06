#if !defined(CETTY_CHANNEL_CHANNELPIPELINEINITIALIZER_H)
#define CETTY_CHANNEL_CHANNELPIPELINEINITIALIZER_H

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
class ChannelPipelineInitializer1 {
public:
    typedef typename ChannelHandlerWrapper<T>::Handler Handler;
    typedef typename ChannelHandlerWrapper<T>::HandlerPtr HandlerPtr;
    typedef typename ChannelHandlerWrapper<T>::Handler::Context Context;

public:
    ChannelPipelineInitializer1()
        : name_("_auto1") {
    }

    ChannelPipelineInitializer1(const std::string& name)
        : name_(name) {
    }

    bool operator()(ChannelPipeline& pipeline) {
        pipeline.addLast<HandlerPtr>(name_, HandlerPtr(new Handler));
        return true;
    }

    template<typename Arg1>
    bool operator()(ChannelPipeline& pipeline, Arg1 arg1) {
        pipeline.addLast<HandlerPtr>(name_, HandlerPtr(new Handler(arg1)));
        return true;
    }

    template<typename Arg1, typename Arg2>
    bool operator()(ChannelPipeline& pipeline, Arg1 arg1, Arg2 arg2) {
        pipeline.addLast<HandlerPtr>(name_, HandlerPtr(new Handler(arg1, arg2)));
        return true;
    }

    template<typename Arg1, typename Arg2, typename Arg3>
    bool operator()(ChannelPipeline& pipeline, Arg1 arg1, Arg2 arg2, Arg3 arg3) {
        pipeline.addLast<HandlerPtr>(name_, HandlerPtr(new Handler(arg1, arg2, arg3)));
        return true;
    }

private:
    std::string name_;
};

template<typename T1, typename T2>
class ChannelPipelineInitializer2 {
public:
    typedef typename ChannelHandlerWrapper<T1>::Handler Handler1;
    typedef typename ChannelHandlerWrapper<T1>::HandlerPtr Handler1Ptr;
    typedef typename ChannelHandlerWrapper<T1>::Handler::Context Context1;

    typedef typename ChannelHandlerWrapper<T2>::Handler Handler2;
    typedef typename ChannelHandlerWrapper<T2>::HandlerPtr Handler2Ptr;
    typedef typename ChannelHandlerWrapper<T2>::Handler::Context Context2;

public:
    ChannelPipelineInitializer2() {}

    bool operator()(ChannelPipeline& pipeline) {
        pipeline.addLast<Handler1Ptr>("_auto1", Handler1Ptr(new Handler1));
        pipeline.addLast<Handler2Ptr>("_auto2", Handler2Ptr(new Handler2));
        return true;
    }
};

template<typename T1, typename T2, typename T3>
class ChannelPipelineInitializer3 {
public:
    typedef typename ChannelHandlerWrapper<T1>::Handler Handler1;
    typedef typename ChannelHandlerWrapper<T1>::HandlerPtr Handler1Ptr;
    typedef typename ChannelHandlerWrapper<T1>::Handler::Context Context1;

    typedef typename ChannelHandlerWrapper<T2>::Handler Handler2;
    typedef typename ChannelHandlerWrapper<T2>::HandlerPtr Handler2Ptr;
    typedef typename ChannelHandlerWrapper<T2>::Handler::Context Context2;

    typedef typename ChannelHandlerWrapper<T3>::Handler Handler3;
    typedef typename ChannelHandlerWrapper<T3>::HandlerPtr Handler3Ptr;
    typedef typename ChannelHandlerWrapper<T3>::Handler::Context Context3;

public:
    ChannelPipelineInitializer3() {}

    bool operator()(ChannelPipeline& pipeline) {
        pipeline.addLast<Handler1Ptr>("_auto1", Handler1Ptr(new Handler1));
        pipeline.addLast<Handler2Ptr>("_auto2", Handler2Ptr(new Handler2));
        pipeline.addLast<Handler3Ptr>("_auto3", Handler3Ptr(new Handler3));
        return true;
    }
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELPIPELINEINITIALIZER_H)

// Local Variables:
// mode: c++
// End:
