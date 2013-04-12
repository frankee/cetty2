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

#include <vector>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/InetAddress.h>
#include <cetty/channel/ChannelHandlerWrapper.h>

#include <cetty/bootstrap/Bootstrap.h>
#include <cetty/bootstrap/ServerBootstrapPtr.h>

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
 * {@link #bind()} and {@link #bind(InetAddress)}.
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
 * b.setOption("localAddress", new {@link InetInetAddress}(8080));
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

class ServerBootstrap : public Bootstrap<ServerBootstrap> {
public:
    ServerBootstrap();
    ServerBootstrap(int parentThreadCnt, int childThreadCnt = 0);

    ServerBootstrap(const EventLoopPoolPtr& pool);
    ServerBootstrap(const EventLoopPoolPtr& pool,
                    const EventLoopPoolPtr& child);

    virtual ~ServerBootstrap();

    /**
     *
     */
    bool daemonize() const;
    ServerBootstrap& setDaemonize(bool daemon);

    /**
     *
     */
    const std::string pidFileName() const;
    ServerBootstrap& setPidFileName(const std::string& fileName);

    /**
     *
     */
    const EventLoopPoolPtr& childLoopPool() const;
    ServerBootstrap& setChildEventLoopPool(const EventLoopPoolPtr& pool);

    /**
     * Set the {@link EventLoopPool} for the parent (acceptor) and the child (client). These
     * {@link EventLoopPool}'s are used to handle all the events and IO for {@link SocketChannel} and
     * {@link Channel}'s.
     */
    virtual ServerBootstrap& setEventLoopPool(const EventLoopPoolPtr& pool);

    /**
     *
     */
    const ChannelOptions& childOptions() const;

    /**
     *
     */
    ServerBootstrap& setChildOptions(const ChannelOptions& options);

    /**
     * Allow to specify a {@link ChannelOption} which is used for the {@link Channel} instances once they get created
     * (after the acceptor accepted the {@link Channel}). Use a value of <code>null</code> to remove a previous set
     * {@link ChannelOption}.
     */
    ServerBootstrap& setChildOption(const ChannelOption& option,
                                    const ChannelOption::Variant& value);

    /**
     * Return the {@link Channel::Initializer} which set for the child {@link Channel}.
     */
    const Channel::Initializer& childInitializer() const;

    /**
     * Set the {@link Channel::Initializer} which is used to initialize the {@link Channel}
     * after the acceptor accepted.
     */
    ServerBootstrap& setChildInitializer(const Channel::Initializer& initializer);

    /**
     * Set the {@link ChannelHandler} which is used to serve the request for the {@link Channel}'s.
     */
    template<typename T>
    ServerBootstrap& setChildHandler(
        const typename ChannelHandlerWrapper<T>::HandlerPtr& handler) {
        childHandler_.reset(
            new typename ChannelHandlerWrapper<T>::Handler::Context(
                "_user",
                handler));

        return *this;
    }

    /**
     *
     */
    ChannelFuturePtr bind();

    /**
     * Creates a new channel which is bound to the local address with only port.
     * This method is similar to the following code:
     *
     * <pre>
     * {@link ServerBootstrap} b = ...;
     * b.bind(InetAddress(port));
     * </pre>
     *
     * @return a new bound channel which accepts incoming connections
     *         if failed to create a new channel and
     *                      bind it to the local address, return null ChannelPtr
     *
     */
    ChannelFuturePtr bind(int port);

    /**
     * Creates a new channel which is bound to the ip and port.  This method is
     * similar to the following code:
     *
     * <pre>
     * {@link ServerBootstrap} b = ...;
     * b.bind(InetAddress(ip, port));
     * </pre>
     *
     * @return a new bound channel which accepts incoming connections
     *         if failed to create a new channel and
     *                      bind it to the local address, return null ChannelPtr
     *
     */
    ChannelFuturePtr bind(const std::string& ip, int port);

    /**
     * Creates a new channel which is bound to the specified local address.
     *
     * @return a new bound channel which accepts incoming connections
     *         if failed to create a new channel and
     *                      bind it to the local address, return null ChannelPtr
     */
    ChannelFuturePtr bind(const InetAddress& localAddress);


    virtual void shutdown();


    virtual void waitingForExit();

private:
    ChannelPtr newChannel();

    bool initServerChannel(const ChannelPtr& channel);

private:
    bool daemonized_;
    std::string pidFile_;

    EventLoopPoolPtr childPool_;
    ChannelOptions childOptions_;
    Channel::Initializer childInitializer_;
    boost::scoped_ptr<ChannelHandlerContext> childHandler_;
};

inline
bool ServerBootstrap::daemonize() const {
    return daemonized_;
}

inline
ServerBootstrap& ServerBootstrap::setDaemonize(bool deamon) {
    daemonized_ = deamon;
    return *this;
}

inline
const std::string ServerBootstrap::pidFileName() const {
    return pidFile_;
}

inline
ServerBootstrap& ServerBootstrap::setPidFileName(const std::string& fileName) {
    pidFile_ = fileName;
    return *this;
}

inline
const ChannelOptions& ServerBootstrap::childOptions() const {
    return childOptions_;
}

inline
ServerBootstrap& ServerBootstrap::setChildOption(const ChannelOption& option,
        const ChannelOption::Variant& value) {
    childOptions_.setOption(option, value);
    return *this;
}

inline
ServerBootstrap& ServerBootstrap::setChildOptions(const ChannelOptions& options) {
    childOptions_ = options;
    return *this;
}

inline
const EventLoopPoolPtr& ServerBootstrap::childLoopPool() const {
    return childPool_;
}

inline
const Channel::Initializer& ServerBootstrap::childInitializer() const {
    return childInitializer_;
}

inline
ServerBootstrap& ServerBootstrap::setChildEventLoopPool(const EventLoopPoolPtr& pool) {
    childPool_ = pool;
    return *this;
}

inline
ServerBootstrap& ServerBootstrap::setChildInitializer(const Channel::Initializer& initializer) {
    childInitializer_ = initializer;
    return *this;
}

}
}

#endif //#if !defined(CETTY_BOOTSTRAP_SERVERBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
