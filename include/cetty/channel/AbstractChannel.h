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
#include <cetty/channel/ChannelSinkFwd.h>

namespace cetty {
namespace channel {

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

    virtual const ChannelPtr& getParent() const;

    virtual const ChannelFactoryPtr& getFactory() const;

    virtual const ChannelPipelinePtr& getPipeline() const;

    /**
     * Returns the {@link System#identityHashCode(Object) identity hash code}
     * of this channel.
     */
    virtual int hashCode() const;

    /**
     * Compares the {@link #getId() ID} of the two channels.
     */
    virtual int compareTo(const ChannelPtr& c) const;

    virtual bool isOpen() const;

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress);

    virtual ChannelFuturePtr unbind();

    virtual ChannelFuturePtr close();

    virtual ChannelFuturePtr& getCloseFuture();

    /**
     * Returns the cached {@link SucceededChannelFuture} instance.
     */
    virtual ChannelFuturePtr& getSucceededFuture();

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress);

    virtual ChannelFuturePtr disconnect();

    virtual int getInterestOps() const;

    virtual ChannelFuturePtr setInterestOps(int interestOps);

    virtual bool isReadable() const;

    virtual bool isWritable() const;

    virtual ChannelFuturePtr setReadable(bool readable);

    virtual ChannelFuturePtr write(const ChannelMessage& message);

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress);

    /**
     * Returns the {@link std::string} representation of this channel.  The returned
     * string contains the @link #getId() ID}, {@link #getLocalAddress() local address},
     * and {@link #getRemoteAddress() remote address} of this channel for
     * easier identification.
     */
    std::string toString() const;

    //TODO: should use a concurrent hash map.
    typedef std::map<int, ChannelPtr> ChannelMap;

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
    AbstractChannel(const ChannelPtr& parent,
                    const ChannelFactoryPtr& factory,
                    const ChannelPipelinePtr& pipeline,
                    const ChannelSinkPtr& sink);

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
                    const ChannelPtr& parent,
                    const ChannelFactoryPtr& factory,
                    const ChannelPipelinePtr& pipeline,
                    const ChannelSinkPtr& sink);

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

    /**
     * Sets the {@link #getInterestOps() interestOps} property of this channel
     * immediately.  This method is intended to be called by an internal
     * component - please do not call it unless you know what you are doing.
     */
    virtual void setInterestOpsNow(int interestOps);

private:
    void idDeallocatorCallback(ChannelFuture& future);
    int allocateId(const ChannelPtr& channel);

private:
    void init(const ChannelPipelinePtr& pipeline, const ChannelSinkPtr& sink);

private:
    static ChannelMap allChannels;

protected:
    int id;

    ChannelPtr parent; // just reference, do not maintenance it's life cycle
    ChannelFactoryPtr  factory; // just reference.
    ChannelPipelinePtr pipeline; // own pipeline, and maintenance life cycle.

    ChannelFuturePtr succeededFuture;
    ChannelFuturePtr closeFuture;

    int interestOps;

    /** Cache for the string representation of this channel */
    mutable std::string strVal;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_ABSTRACTCHANNEL_H)

// Local Variables:
// mode: c++
// End:
