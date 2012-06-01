#if !defined(CETTY_CHANNEL_CHANNELSINK_H)
#define CETTY_CHANNEL_CHANNELSINK_H

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

#include <cetty/channel/ChannelSinkFwd.h>

namespace cetty {
namespace channel {

class ChannelEvent;
class MessageEvent;
class ChannelStateEvent;

class ChannelPipeline;
class ChannelPipelineException;

/**
 * Receives and processes the terminal downstream {@link ChannelEvent}s.
 * <p>
 * A {@link ChannelSink} is an internal component which is supposed to be
 * implemented by a transport provider.  Most users will not see this type
 * in their code.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.uses org.jboss.netty.channel.ChannelPipeline - - sends events upstream
 */

class ChannelSink {
public:
    virtual ~ChannelSink() {}

    /**
     * Invoked by {@link ChannelPipeline} when a downstream {@link ChannelEvent},
     * except the {@link MessageEvent} and {@link ChannelStateEvent},
     * has reached its terminal (the head of the pipeline).
     */
    virtual void eventSunk(const ChannelPipeline& pipeline,
                            const ChannelEvent& e) = 0;

    /**
     * Invoked by {@link ChannelPipeline} when a downstream {@link MessageEvent}
     * has reached its terminal (the head of the pipeline).
     */
    virtual void writeRequested(const ChannelPipeline& pipeline,
                                 const MessageEvent& e) = 0;

    /**
     * Invoked by {@link ChannelPipeline} when a downstream {@link ChannelStateEvent}
     * has reached its terminal (the head of the pipeline).
     */
    virtual void stateChangeRequested(const ChannelPipeline& pipeline,
                                       const ChannelStateEvent& e) = 0;

    /**
     * Invoked by {@link ChannelPipeline} when an exception was raised while
     * one of its {@link ChannelHandler}s process a {@link ChannelEvent}.
     */
    virtual void exceptionCaught(const ChannelPipeline& pipeline,
                                  const ChannelEvent& e,
                                  const ChannelPipelineException& cause) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELSINK_H)

// Local Variables:
// mode: c++
// End:

