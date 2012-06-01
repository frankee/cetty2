#if !defined(CETTY_CHANNEL_LIFECYCLEAWARECHANNELHANDLER_H)
#define CETTY_CHANNEL_LIFECYCLEAWARECHANNELHANDLER_H

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

#include <cetty/channel/ChannelHandler.h>
#include <cetty/channel/LifeCycleAwareChannelHandlerFwd.h>

namespace cetty {
namespace channel {

/**
 * A {@link ChannelHandler} that is notified when it is added to or removed
 * from a {@link ChannelPipeline}.
 *
 * <h3>Invalid access to the {@link ChannelHandlerContext}</h3>
 *
 * Calling {@link ChannelHandlerContext#sendUpstream(ChannelEvent)} or
 * {@link ChannelHandlerContext#sendDownstream(ChannelEvent)} in
 * {@link #beforeAdd(ChannelHandlerContext)} or {@link #afterRemove(ChannelHandlerContext)}
 * might lead to an unexpected behavior.  It is because the context object
 * might not have been fully added to the pipeline or the context object is not
 * a part of the pipeline anymore respectively.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class LifeCycleAwareChannelHandler : virtual public ChannelHandler {
public:
    virtual ~LifeCycleAwareChannelHandler() {}

    /**
     *
     */
    virtual void beforeAdd(ChannelHandlerContext& ctx) = 0;

    /**
     *
     */
    virtual void afterAdd(ChannelHandlerContext& ctx) = 0;

    /**
     *
     */
    virtual void beforeRemove(ChannelHandlerContext& ctx) = 0;

    /**
     *
     */
    virtual void afterRemove(ChannelHandlerContext& ctx) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_LIFECYCLEAWARECHANNELHANDLER_H)

// Local Variables:
// mode: c++
// End:

