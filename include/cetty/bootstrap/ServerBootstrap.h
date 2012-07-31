#if !defined(CETTY_BOOTSTRAP_SERVERBOOTSTRAP_H)
#define CETTY_BOOTSTRAP_SERVERBOOTSTRAP_H

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

#include <deque>

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelHandler.h>

#include <cetty/bootstrap/Bootstrap.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;

/**
 * A helper class which creates a new server-side {@link Channel} and accepts
 * incoming connections.
 *
 * <h3>Only for connection oriented transports</h3>
 *
 * This bootstrap is for connection oriented transports only such as TCP/IP
 * and local transport.  Use {@link ConnectionlessBootstrap} instead for
 * connectionless transports.  Do not use this helper if you are using a
 * connectionless transport such as UDP/IP which does not accept an incoming
 * connection but receives messages by itself without creating a child channel.
 *
 * <h3>Parent channel and its children</h3>
 *
 * A parent channel is a channel which is supposed to accept incoming
 * connections.  It is created by this bootstrap's {@link ChannelFactory} via
 * {@link #bind()} and {@link #bind(SocketAddress)}.
 * <p>
 * Once successfully bound, the parent channel starts to accept incoming
 * connections, and the accepted connections become the children of the
 * parent channel.
 *
 * <h3>Configuring channels</h3>
 *
 * {@link #setOption(std::string, Object) Options} are used to configure both a
 * parent channel and its child channels.  To configure the child channels,
 * prepend <tt>"child."</tt> prefix to the actual option names of a child
 * channel:
 *
 * <pre>
 * {@link ServerBootstrap} b = ...;
 *
 * // Options for a parent channel
 * b.setOption("localAddress", new {@link InetSocketAddress}(8080));
 * b.setOption("reuseAddress", true);
 *
 * // Options for its children
 * b.setOption("child.tcpNoDelay", true);
 * b.setOption("child.receiveBufferSize", 1048576);
 * </pre>
 *
 * For the detailed list of available options, please refer to
 * {@link ChannelConfig} and its sub-types.
 *
 * <h3>Configuring a parent channel pipeline</h3>
 *
 * It is rare to customize the pipeline of a parent channel because what it is
 * supposed to do is very typical.  However, you might want to add a handler
 * to deal with some special needs such as degrading the process
 * <a href="http://en.wikipedia.org/wiki/User_identifier_(Unix)">UID</a> from
 * a <a href="http://en.wikipedia.org/wiki/Superuser">superuser</a> to a
 * normal user and changing the current VM security manager for better
 * security.  To support such a case,
 * the {@link #setParentHandler(ChannelHandler) parentHandler} property is
 * provided.
 *
 * <h3>Configuring a child channel pipeline</h3>
 *
 * Every channel has its own {@link ChannelPipeline} and you can configure it
 * in two ways.
 *
 * The recommended approach is to specify a {@link ChannelPipelineFactory} by
 * calling {@link #setPipelineFactory(ChannelPipelineFactory)}.
 *
 * <pre>
 * {@link ServerBootstrap} b = ...;
 * b.setPipelineFactory(new MyPipelineFactory());
 *
 * public class MyPipelineFactory implements {@link ChannelPipelineFactory} {
 *   public {@link ChannelPipeline} getPipeline() throws Exception {
 *     // Create and configure a new pipeline for a new channel.
 *     {@link ChannelPipeline} p = {@link Channels}.pipeline();
 *     p.addLast("encoder", new EncodingHandler());
 *     p.addLast("decoder", new DecodingHandler());
 *     p.addLast("logic",   new LogicHandler());
 *     return p;
 *   }
 * }
 * </pre>

 * <p>
 * The alternative approach, which works only in a certain situation, is to use
 * the default pipeline and let the bootstrap to shallow-copy the default
 * pipeline for each new channel:
 *
 * <pre>
 * {@link ServerBootstrap} b = ...;
 * {@link ChannelPipeline} p = b.getPipeline();
 *
 * // Add handlers to the default pipeline.
 * p.addLast("encoder", new EncodingHandler());
 * p.addLast("decoder", new DecodingHandler());
 * p.addLast("logic",   new LogicHandler());
 * </pre>
 *
 * Please note 'shallow-copy' here means that the added {@link ChannelHandler}s
 * are not cloned but only their references are added to the new pipeline.
 * Therefore, you cannot use this approach if you are going to open more than
 * one {@link Channel}s or run a server that accepts incoming connections to
 * create its child channels.
 *
 * <h3>Applying different settings for different {@link Channel}s</h3>
 *
 * {@link ServerBootstrap} is just a helper class.  It neither allocates nor
 * manages any resources.  What manages the resources is the
 * {@link ChannelFactory} implementation you specified in the constructor of
 * {@link ServerBootstrap}.  Therefore, it is OK to create as many
 * {@link ServerBootstrap} instances as you want with the same
 * {@link ChannelFactory} to apply different settings for different
 * {@link Channel}s.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */

class ServerBootstrap : public Bootstrap {
public:
    /**
     * Creates a new instance with no {@link ChannelFactory} set.
     * {@link #setFactory(ChannelFactory)} must be called before any I/O
     * operation is requested.
     */
    ServerBootstrap() {}

    /**
     * Creates a new instance with the specified initial {@link ChannelFactory}.
     */
    ServerBootstrap(const ChannelFactoryPtr& channelFactory)
        : Bootstrap(channelFactory) {
    }

    virtual ~ServerBootstrap() {}

    virtual ServerBootstrap& setFactory(const ChannelFactoryPtr& factory);
    virtual ServerBootstrap& setPipeline(const ChannelPipelinePtr& pipeline);

    ServerBootstrap& setChildOption(const ChannelOption& option,
        const ChannelOption::Variant& value);

    const ChannelOption::Options& getChildOptions() const;

    /**
     * Returns an optional {@link ChannelHandler} which intercepts an event
     * of a newly bound server-side channel which accepts incoming connections.
     *
     * @return the parent channel handler.
     *         <tt>NULL</tt> if no parent channel handler is set.
     */
    const ChannelHandlerPtr& getParentHandler();

    /**
     * Sets an optional {@link ChannelHandler} which intercepts an event of
     * a newly bound server-side channel which accepts incoming connections.
     *
     * @param parentHandler
     *        the parent channel handler.
     *        <tt>NULL</tt> to unset the current parent channel handler.
     */
    ServerBootstrap& setParentHandler(const ChannelHandlerPtr& parentHandler);

    /**
     * Creates a new channel which is bound to the local address with only port.
     * This method is similar to the following code:
     *
     * <pre>
     * {@link ServerBootstrap} b = ...;
     * b.bind(SocketAddress(port));
     * </pre>
     *
     * @return a new bound channel which accepts incoming connections
     *         if failed to create a new channel and
     *                      bind it to the local address, return null ChannelPtr
     *
     */
    virtual ChannelFuturePtr bind(int port);

    /**
     * Creates a new channel which is bound to the ip and port.  This method is
     * similar to the following code:
     *
     * <pre>
     * {@link ServerBootstrap} b = ...;
     * b.bind(SocketAddress(ip, port));
     * </pre>
     *
     * @return a new bound channel which accepts incoming connections
     *         if failed to create a new channel and
     *                      bind it to the local address, return null ChannelPtr
     *
     */
    virtual ChannelFuturePtr bind(const std::string& ip, int port);

    /**
     * Creates a new channel which is bound to the specified local address.
     *
     * @return a new bound channel which accepts incoming connections
     *         if failed to create a new channel and
     *                      bind it to the local address, return null ChannelPtr
     */
    virtual ChannelFuturePtr bind(const SocketAddress& localAddress);

private:
    ChannelHandlerPtr parentHandler;
    ChannelOption::Options childOptions;
};

}
}

#endif //#if !defined(CETTY_BOOTSTRAP_SERVERBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:

