#if !defined(CETTY_CHANNEL_DOWNSTREAMCHANNELSTATEEVENT_H)
#define CETTY_CHANNEL_DOWNSTREAMCHANNELSTATEEVENT_H
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

#include <boost/any.hpp>
#include <cetty/channel/ChannelStateEvent.h>
#include <cetty/channel/ChannelState.h>

namespace cetty {
namespace channel {

/**
 * The default downstream {@link ChannelStateEvent} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class DownstreamChannelStateEvent : public ChannelStateEvent {
public:
    /**
     * Creates a new instance.
     */
    DownstreamChannelStateEvent(const ChannelPtr& channel,
                                const ChannelFuturePtr& future,
                                const ChannelState& state,
                                const boost::any& value);

    DownstreamChannelStateEvent(const ChannelPtr& channel,
                                const ChannelFuturePtr& future,
                                const ChannelState& state);

    virtual ~DownstreamChannelStateEvent();

    virtual const ChannelPtr& getChannel() const;
    virtual const ChannelFuturePtr& getFuture() const;
    virtual const ChannelState& getState() const;
    virtual const boost::any& getValue() const;

    virtual std::string toString() const;

private:
    DownstreamChannelStateEvent(const DownstreamChannelStateEvent&);
    const DownstreamChannelStateEvent& operator=(const DownstreamChannelStateEvent&);

private:
    const ChannelPtr&       channel;
    const ChannelFuturePtr& future;
    ChannelState state;
    boost::any   value;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_DOWNSTREAMCHANNELSTATEEVENT_H)

// Local Variables:
// mode: c++
// End:

