#if !defined(CETTY_CHANNEL_ABSTRACTCHANNEL_H)
#define CETTY_CHANNEL_ABSTRACTCHANNEL_H

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

#include <string>
#include <map>
#include <cetty/channel/Channel.h>
#include <cetty/channel/EventLoopPtr.h>

namespace cetty {
namespace channel {

class ChannelSinkHandler;

/**
 * A skeletal {@link Channel} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class AbstractChannel : public Channel {
public:
    virtual ~AbstractChannel();

    virtual int getId() const;

    virtual const EventLoopPtr& getEventLoop() const;

    virtual const ChannelPtr& getParent() const;

    virtual const ChannelFactoryPtr& getFactory() const;

    virtual const ChannelPipelinePtr& getPipeline() const;

    virtual bool isOpen() const;

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress);

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress);

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress,
                                     const SocketAddress& localAddress);

    virtual ChannelFuturePtr disconnect();
    virtual ChannelFuturePtr close();
    virtual ChannelFuturePtr flush();

    virtual const ChannelFuturePtr& bind(const SocketAddress& localAddress,
                                         const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const SocketAddress& localAddress,
                                            const ChannelFuturePtr& future);
    virtual const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future);
    virtual const ChannelFuturePtr& close(const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& flush(const ChannelFuturePtr& future);

    virtual ChannelFuturePtr& getCloseFuture();

    virtual ChannelFuturePtr newFuture();
    virtual ChannelFuturePtr newFailedFuture(const Exception& e);

    /**
     * Returns the cached {@link SucceededChannelFuture} instance.
     */
    virtual ChannelFuturePtr newSucceededFuture();

    /**
     * Returns the {@link std::string} representation of this channel.  The returned
     * string contains the @link #getId() ID}, {@link #getLocalAddress() local address},
     * and {@link #getRemoteAddress() remote address} of this channel for
     * easier identification.
     */
    std::string toString() const;

    /**
     * Compares the {@link #getId() ID} of the two channels.
     */
    virtual int compareTo(const ChannelPtr& c) const;

    //TODO: should use a concurrent hash map.
    typedef std::map<int, ChannelPtr> ChannelMap;

    virtual void doPreClose() {} // NOOP by default

protected:
    /**
     * Creates a new instance.
     *
     * @param parent
     *        the parent of this channel. <tt>NULL</tt> if there's no parent.
     * @param factory
     *        the factory which created this channel
     * @param pipeline
     *        the pipeline which is going to be attached to this channel
     * @param sink
     *        the sink which will receive downstream events from the pipeline
     *        and send upstream events to the pipeline
     */
    AbstractChannel(const EventLoopPtr& eventLoop,
                    const ChannelPtr& parent,
                    const ChannelFactoryPtr& factory,
                    const ChannelPipelinePtr& pipeline);

    /**
     * (Internal use only) Creates a new temporary instance with the specified
     * ID.
     *
     * @param parent
     *        the parent of this channel. <tt>NULL</tt> if there's no parent.
     * @param factory
     *        the factory which created this channel
     * @param pipeline
     *        the pipeline which is going to be attached to this channel
     * @param sink
     *        the sink which will receive downstream events from the pipeline
     *        and send upstream events to the pipeline

    */
    AbstractChannel(int id,
                    const EventLoopPtr& eventLoop,
                    const ChannelPtr& parent,
                    const ChannelFactoryPtr& factory,
                    const ChannelPipelinePtr& pipeline);

    virtual void doBind(const SocketAddress& localAddress) = 0;
    virtual void doDisconnect() = 0;
    virtual void doClose() = 0;

    /**
     * Marks this channel as closed.  This method is intended to be called by
     * an internal component - please do not call it unless you know what you
     * are doing.
     *
     * @return <tt>true</tt> if and only if this channel was not marked as


    *                      closed yet
     */
    virtual bool setClosed();

    /**
     * Returns the {@link FailedChannelFuture} whose cause is an
     * {@link UnsupportedOperationException}.
     */
    virtual ChannelFuturePtr getUnsupportedOperationFuture();

    virtual void setPipeline(const ChannelPipelinePtr& pipeline);

private:
    void idDeallocatorCallback(ChannelFuture& future);
    int  allocateId(const ChannelPtr& channel);

private:
    friend class ChannelSinkHandler;

    static ChannelMap allChannels;

protected:
    bool active;
    int id;

    EventLoopPtr eventLoop;
    ChannelPtr parent; // just reference, do not maintenance it's life cycle
    ChannelFactoryPtr  factory; // just reference.
    ChannelPipelinePtr pipeline;

    ChannelFuturePtr succeededFuture;
    ChannelFuturePtr closeFuture;

    /** Cache for the string representation of this channel */
    mutable std::string strVal;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_ABSTRACTCHANNEL_H)

// Local Variables:
// mode: c++
// End:
