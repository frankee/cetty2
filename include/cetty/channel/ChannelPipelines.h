#if !defined(CETTY_CHANNEL_CHANNELPIPELINES_H)
#define CETTY_CHANNEL_CHANNELPIPELINES_H

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

#include <vector>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelHandler.h>
#include <cetty/channel/ChannelPipelineFactory.h>

namespace cetty {
namespace util {
class Exception;
}
}

namespace cetty {
namespace channel {

using namespace cetty::util;

class Channel;
class ChannelPipeline;
class ChannelHandler;
class ChannelHandlerContext;
class ChannelFuture;
class UserEvent;

class SocketAddress;

/**
 * A helper class which provides various convenience methods related with
 * {@link Channel}, {@link ChannelHandler}, and {@link ChannelPipeline}.
 *
 * <h3>Factory methods</h3>
 * <p>
 * It is always recommended to use the factory methods provided by
 * {@link Channels} rather than calling the constructor of the implementation
 * types.
 * <ul>
 * <li>{@link #pipeline()}</li>
 * <li>{@link #pipeline(ChannelPipeline)}</li>
 * <li>{@link #pipelineFactory(ChannelPipeline)}</li>
 * <li>{@link #succeededFuture(Channel)}</li>
 * <li>{@link #failedFuture(Channel, Throwable)}</li>
 * </ul>
 *
 * <h3>Upstream and downstream event generation</h3>
 * <p>
 * Various event generation methods are provided to simplify the generation of
 * upstream events and downstream events.  It is always recommended to use the
 * event generation methods provided by {@link Channels} rather than calling
 * {@link ChannelHandlerContext#sendUpstream(ChannelEvent)} or
 * {@link ChannelHandlerContext#sendDownstream(ChannelEvent)} by yourself.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @apiviz.landmark
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class ChannelPipelines {
public:
    /**
     * Creates a new {@link ChannelPipeline}.
     */
    static ChannelPipelinePtr pipeline();

    /**
     * Creates a new {@link ChannelPipeline} which contains the specified
     * {@link ChannelHandler}s.  The names of the specified handlers are
     * generated automatically; the first handler's name is <tt>"0"</tt>,
     * the second handler's name is <tt>"1"</tt>, the third handler's name is
     * <tt>"2"</tt>, and so on.
     */
    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler);

    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler0,
                                     const ChannelHandlerPtr& handler1);

    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler0,
                                     const ChannelHandlerPtr& handler1,
                                     const ChannelHandlerPtr& handler2);

    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler0,
                                     const ChannelHandlerPtr& handler1,
                                     const ChannelHandlerPtr& handler2,
                                     const ChannelHandlerPtr& handler3);

    static ChannelPipelinePtr pipeline(const std::vector<ChannelHandlerPtr>& handlers);

    /**
     * Creates a new {@link ChannelPipeline} which contains the same entries
     * with the specified <tt>pipeline</tt>.  Please note that only the names
     * and the references of the {@link ChannelHandler}s will be copied; a new
     * {@link ChannelHandler} instance will never be created.
     */
    static ChannelPipelinePtr pipeline(const ChannelPipelinePtr& pipeline);

private:

    ChannelPipelines() {}
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELPIPELINES_H)

// Local Variables:
// mode: c++
// End:

