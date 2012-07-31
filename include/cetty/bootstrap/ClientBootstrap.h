#if !defined(CETTY_BOOTSTRAP_CLIENTBOOTSTRAP_H)
#define CETTY_BOOTSTRAP_CLIENTBOOTSTRAP_H

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
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 *
 */

#include <cetty/channel/ChannelFutureFwd.h>
#include <cetty/bootstrap/Bootstrap.h>

namespace cetty {
    namespace channel {
        class SocketAddress;
    }
}

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;

/**
 * A helper class which creates a new client-side {@link Channel} and makes a
 * connection attempt.
 *
 * <h3>Configuring a channel</h3>
 *
 * {@link #const std::string&, const boost::any&) Options} are used to configure a channel:
 *
 * <pre>
 * {@link ClientBootstrap} b = ...;
 *
 * // Options for a new channel
 * b.setOption("remoteAddress", boost::any({@link SocketAddress}("example.com", 8080)));
 * b.setOption("tcpNoDelay", true);
 * b.setOption("receiveBufferSize", 1048576);
 * </pre>
 *
 * For the detailed list of available options, please refer to
 * {@link ChannelConfig} and its sub-types.
 *
 * <h3>Configuring a channel pipeline</h3>
 *
 * Every channel has its own {@link ChannelPipeline} and you can configure it
 * in two ways.
 *
 * The recommended approach is to specify a {@link ChannelPipelineFactory} by
 * calling {@link #setPipelineFactory(ChannelPipelineFactory)}.
 *
 * <pre>
 * {@link ClientBootstrap} b = ...;
 * b.setPipelineFactory(ChannelPipelinePtr(new MyPipelineFactory()));
 *
 * class MyPipelineFactory : public {@link ChannelPipelineFactory} {
 * public:
 *     virtual ~MyPipelineFactory() {}
 *
 *     virtual {@link ChannelPipeline} getPipeline() {
 *         // Create and configure a new pipeline for a new channel.
 *         {@link ChannelPipeline} *p = {@link Channels}.pipeline();
 *         p.addLast("encoder", ChannelHandlerPtr(new EncodingHandler()));
 *         p.addLast("decoder", ChannelHandlerPtr(new DecodingHandler()));
 *         p.addLast("logic",   ChannelHandlerPtr(new LogicHandler()));
 *         return p;
 *    }
 * }
 * </pre>
 *
 * <p>
 * The alternative approach, which works only in a certain situation, is to use
 * the default pipeline and let the bootstrap to shallow-copy the default
 * pipeline for each new channel:
 *
 * <pre>
 * {@link ClientBootstrap} b = ...;
 * {@link ChannelPipeline} *p = b.getPipeline();
 *
 * // Add handlers to the default pipeline.
 * p->addLast("encoder", ChannelHandlerPtr(new EncodingHandler()));
 * p->addLast("decoder", ChannelHandlerPtr(new DecodingHandler()));
 * p->addLast("logic",   ChannelHandlerPtr(new LogicHandler()));
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
 * {@link ClientBootstrap} is just a helper class.  It neither allocates nor
 * manages any resources.  What manages the resources is the
 * {@link ChannelFactory} implementation you specified in the constructor of
 * {@link ClientBootstrap}.  Therefore, it is OK to create as many
 * {@link ClientBootstrap} instances as you want with the same
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

class ClientBootstrap : public Bootstrap {
public:
    /**
     * Creates a new instance with no {@link ChannelFactory} set.
     * {@link #setFactory(ChannelFactory)} must be called before any I/O
     * operation is requested.
     */
    ClientBootstrap();

    /**
     * Creates a new instance with the specified initial {@link ChannelFactory}.
     */
    ClientBootstrap(const ChannelFactoryPtr& channelFactory);

    virtual ~ClientBootstrap() {}

    /**
     * Attempts a new connection with the current <tt>"remoteAddress"</tt> and
     * <tt>"localAddress"</tt> option.  If the <tt>"localAddress"</tt> option is
     * not set, the local address of a new channel is determined automatically.
     * This method is similar to the following code:
     *
     * <pre>
     * {@link ClientBootstrap} b = ...;
     * b.connect(b.getOption("remoteAddress"), b.getOption("localAddress"));
     * </pre>
     *
     * @return a future object which notifies when this connection attempt
     *         succeeds or fails
     *
     * @throws IllegalStateException
     *         if <tt>"remoteAddress"</tt> option was not set
     * @throws ChannelPipelineException
     *         if this bootstrap's {@link #setPipelineFactory(ChannelPipelineFactory) pipelineFactory}
     *            failed to create a new {@link ChannelPipeline}
     */
    ChannelFuturePtr connect(const std::string& host, int port);

    /**
     * Attempts a new connection with the specified <tt>remoteAddress</tt> and
     * the current <tt>"localAddress"</tt> option. If the <tt>"localAddress"</tt>
     * option is not set, the local address of a new channel is determined
     * automatically.  This method is identical with the following code:
     *
     * <pre>
     * {@link ClientBootstrap} b = ...;
     * b.connect(remoteAddress, b.getOption("localAddress"));
     * </pre>
     *
     * @return a future object which notifies when this connection attempt
     *         succeeds or fails
     *
     * @throws ChannelPipelineException
     *         if this bootstrap's {@link #setPipelineFactory(ChannelPipelineFactory) pipelineFactory}
     *            failed to create a new {@link ChannelPipeline}
     */
    ChannelFuturePtr connect(const SocketAddress& remoteAddress);

    /**
     * Attempts a new connection with the specified <tt>remoteAddress</tt> and
     * the specified <tt>localAddress</tt>.  If the specified local address is
     * <tt>null</tt>, the local address of a new channel is determined
     * automatically.
     *
     * @return a future object which notifies when this connection attempt
     *         succeeds or fails
     *
     * @throws ChannelPipelineException
     *         if this bootstrap's {@link #setPipelineFactory(ChannelPipelineFactory) pipelineFactory}
     *            failed to create a new {@link ChannelPipeline}
     */
    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress);
};

}
}

#endif //#if !defined(CETTY_BOOTSTRAP_CLIENTBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:

