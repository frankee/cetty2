#if !defined(CETTY_CHANNEL_CHANNELFACTORY_H)
#define CETTY_CHANNEL_CHANNELFACTORY_H

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

#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/ChannelFactoryFwd.h>
#include <cetty/channel/ChannelPipelineFwd.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/util/ExternalResourceReleasable.h>

namespace cetty {
namespace channel {

/**
 * The main interface to a transport that creates a {@link Channel} associated
 * with a certain communication entity such as a network socket.  For example,
 * the {@link AsioServerSocketChannelFactory} creates a channel which has a
 * ASIO-based server socket as its underlying communication entity.
 * <p>
 * Once a new {@link Channel} is created, the {@link ChannelPipeline} which
 * was specified as a parameter in the {@link #newChannel(ChannelPipeline*)}
 * is attached to the new {@link Channel}, and starts to handle all associated
 * {@link ChannelEvent}s.
 *
 * <h3>Graceful shutdown</h3>
 * <p>
 * To shut down a network application service which is managed by a factory.
 * you should follow the following steps:
 * <ol>
 * <li>close all channels created by the factory and their child channels, and</li>
 * <li>call {@link #releaseExternalResources()}.</li>
 * </ol>
 * <p>
 * For detailed transport-specific information on shutting down a factory,
 * please refer to the {@link ChannelFactory}'s subtypes, such as
 * {@link AsioServerSocketChannelFactory}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ChannelFactory
    : public cetty::util::ReferenceCounter<ChannelFactory, int> {
public:
    ChannelFactory() {}
    virtual ~ChannelFactory() {}

    /**
     * Creates and opens a new {@link Channel} and attaches the specified
     * {@link ChannelPipeline} to the new {@link Channel}.
     *
     * @param pipeline the {@link ChannelPipeline} which is going to be
     *                 attached to the new {@link Channel}
     *
     * @return the newly open channel, if failed, return the NULL.
     *
     */
    virtual ChannelPtr newChannel(const ChannelPipelinePtr& pipeline) = 0;

    virtual void shutdown() = 0;

private:
    ChannelFactory(const ChannelFactory&);
    ChannelFactory& operator=(const ChannelFactory&);
};

}
}


#endif //#if !defined(CETTY_CHANNEL_CHANNELFACTORY_H)

// Local Variables:
// mode: c++
// End:

