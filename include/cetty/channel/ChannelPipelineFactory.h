#if !defined(CETTY_CHANNEL_CHANNELPIPELINEFACTORY_H)
#define CETTY_CHANNEL_CHANNELPIPELINEFACTORY_H

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

#include <cetty/util/ReferenceCounter.h>
#include <cetty/channel/ChannelPipelinePtr.h>
#include <cetty/channel/ChannelPipelineFactoryPtr.h>

namespace cetty {
namespace channel {

/**
 * Creates a new {@link ChannelPipeline} for a new {@link Channel}.
 * <p>
 * When a {@link ServerChannel server-side channel} accepts a new incoming
 * connection, a new child channel is created for each newly accepted connection.
 * A new child channel uses a new {@link ChannelPipeline}, which is created by
 * the {@link ChannelPipelineFactory} specified in the server-side channel's
 * {@link ChannelConfig#getPipelineFactory() "pipelineFactory"} option.
 * <p>
 * Also, when a {@link ClientBootstrap} or {@link ConnectionlessBootstrap}
 * creates a new channel, it uses the {@link Bootstrap#getPipelineFactory() "pipelineFactory"}
 * property to create a new {@link ChannelPipeline} for each new channel.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.has org.jboss.netty.channel.ChannelPipeline oneway - - creates
 */

class ChannelPipelineFactory : public cetty::util::ReferenceCounter<ChannelPipelineFactory, int> {
public:
    virtual ~ChannelPipelineFactory() {}

    /**
     * Returns a newly created {@link ChannelPipeline}.
     */
    virtual ChannelPipelinePtr getPipeline() = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELPIPELINEFACTORY_H)

// Local Variables:
// mode: c++
// End:
