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

#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelHandlerWrapper.h>
#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace channel {

class ChannelInitializer {
public:
    template<T>
    ChannelInitializer(const ChannelHandlerWrapper<T>::HandlerPtr& t) {
        contexts_.push_back(new typename T::Context(t));
    }

    template<T1, T2>
    ChannelInitializer(const ChannelHandlerWrapper<T1>::HandlerPtr& t1,
                       const ChannelHandlerWrapper<T2>::HandlerPtr& t2) {
        contexts_.push_back(new typename T1::Context(t1));
        contexts_.push_back(new typename T2::Context(t2));
    }

    template<T1, T2, T3>
    ChannelInitializer() {

    }

    template<T1, T2, T3, T4>
    ChannelInitializer() {

    }

    template<T1, T2, T3, T4, T5>
    ChannelInitializer() {

    }

    template<T1, T2, T3, T4, T5, T6>
    ChannelInitializer() {

    }

    template<T1, T2, T3, T4, T5, T6, T7>
    ChannelInitializer() {

    }

    template<T1, T2, T3, T4, T5, T6, T7, T8>
    ChannelInitializer() {

    }

    template<T1, T2, T3, T4, T5, T6, T7, T8, T9>
    ChannelInitializer() {

    }

    bool operator()(const ChannelPtr& channel) {
        const ChannelPipelinePtr pipeline = channel->pipeline();
        pipeline->addLast(contexts_.begin(), contexts_.end());
    }

private:
    std::vector<ChannelHandlerContext*> contexts_;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELINITIALIZER_H)

// Local Variables:
// mode: c++
// End:
