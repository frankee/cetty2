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

class ChannelInitializer {
public:
    template<typename T>
    ChannelInitializer(typename const ChannelHandlerWrapper<T>::HandlerPtr& t) {
        contexts_.push_back(
            new typename ChannelHandlerWrapper<T>::Handler::Context("1", t));
    }

    template<typename T1, typename T2>
    ChannelInitializer(typename const ChannelHandlerWrapper<T1>::HandlerPtr& t1,
                       typename const ChannelHandlerWrapper<T2>::HandlerPtr& t2) {
        contexts_.push_back(
            new typename ChannelHandlerWrapper<T1>::Handler::Context("1", t1));

        contexts_.push_back(
            new typename ChannelHandlerWrapper<T2>::Handler::Context("2", t2));
    }

    template<typename T1, typename T2, typename T3>
    ChannelInitializer(typename const ChannelHandlerWrapper<T1>::HandlerPtr& t1,
        typename const ChannelHandlerWrapper<T2>::HandlerPtr& t2,
        typename const ChannelHandlerWrapper<T3>::HandlerPtr& t3) {
            contexts_.push_back(
                new typename ChannelHandlerWrapper<T1>::Handler::Context("1", t1));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T2>::Handler::Context("2", t2));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T3>::Handler::Context("3", t3));
    }

    template<typename T1, typename T2, typename T3, typename T4>
    ChannelInitializer(typename const ChannelHandlerWrapper<T1>::HandlerPtr& t1,
        typename const ChannelHandlerWrapper<T2>::HandlerPtr& t2,
        typename const ChannelHandlerWrapper<T3>::HandlerPtr& t3,
        typename const ChannelHandlerWrapper<T4>::HandlerPtr& t4) {
            contexts_.push_back(
                new typename ChannelHandlerWrapper<T1>::Handler::Context("1", t1));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T2>::Handler::Context("2", t2));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T3>::Handler::Context("3", t3));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T4>::Handler::Context("4", t4));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5>
    ChannelInitializer(typename const ChannelHandlerWrapper<T1>::HandlerPtr& t1,
        typename const ChannelHandlerWrapper<T2>::HandlerPtr& t2,
        typename const ChannelHandlerWrapper<T3>::HandlerPtr& t3,
        typename const ChannelHandlerWrapper<T4>::HandlerPtr& t4,
        typename const ChannelHandlerWrapper<T5>::HandlerPtr& t5) {
            contexts_.push_back(
                new typename ChannelHandlerWrapper<T1>::Handler::Context("1", t1));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T2>::Handler::Context("2", t2));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T3>::Handler::Context("3", t3));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T4>::Handler::Context("4", t4));

            contexts_.push_back(
                new typename ChannelHandlerWrapper<T5>::Handler::Context("5", t5));
    }

    bool operator()(const ChannelPtr& channel) {
        ChannelPipeline& pipeline = channel->pipeline();

        std::vector<ChannelHandlerContext*>::iterator itr = contexts_.begin();
        for (; itr != contexts_.end(); ++itr) {
            pipeline.addLast(*itr);
        }

        return true;
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
