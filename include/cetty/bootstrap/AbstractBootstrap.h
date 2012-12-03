#if !defined(CETTY_BOOTSTRAP_ABSTRACTBOOTSTRAP_H)
#define CETTY_BOOTSTRAP_ABSTRACTBOOTSTRAP_H
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

#include <boost/noncopyable.hpp>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelOption.h>
#include <cetty/channel/EventLoopPool.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;

/**
 * A helper class which initializes a {@link Channel}.  This class provides
 * the common data structure for its subclasses which actually initialize
 * {@link Channel}s and their child {@link Channel}s using the common data
 * structure.  Please refer to {@link ClientBootstrap}, {@link ServerBootstrap},
 * and {@link ConnectionlessBootstrap} for client side, server-side, and
 * connectionless (e.g. UDP) channel initialization respectively.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.uses cetty::channel::ChannelFactory
 */

template<typename T>
class AbstractBootstrap : private boost::noncopyable {
public:
    virtual ~AbstractBootstrap() {}

    const SocketAddress& localAddress() const;

    /**
     * See {@link #setLocalAddress(const SocketAddress&)}
     */
    T& setLocalAddress(int port);

    /**
     * See {@link #localAddress(const SocketAddress&)}
     */
    T& setLocalAddress(const std::string& host, int port);

    T& setLocalAddress(const SocketAddress& localAddress);

    const EventLoopPoolPtr& eventLoopPool() const;

    T& setEventLoopPool(const EventLoopPoolPtr& pool);

    /**
     * Returns the options which configures a new {@link Channel} and its
     * child {@link Channel}s.  The names of the child {@link Channel} options
     * are prefixed with <tt>"child."</tt> (e.g. <tt>"child.keepAlive"</tt>).
     */
    ChannelOption::Options& options();

    /**
     * Returns the options which configures a new {@link Channel} and its
     * child {@link Channel}s.  The names of the child {@link Channel} options
     * are prefixed with <tt>"child."</tt> (e.g. <tt>"child.keepAlive"</tt>).
     */
    const ChannelOption::Options& options() const;

    /**
     * Returns the value of the option with the specified key.  To retrieve
     * the option value of a child {@link Channel}, prefixed <tt>"child."</tt>
     * to the option name (e.g. <tt>"child.keepAlive"</tt>).
     *
     * @param key  the option name
     *
     * @return the option value if the option is found.
     *         <tt>empty boost::any</tt> otherwise.
     */
    ChannelOption::Variant getOption(const ChannelOption& option) const;

    /**
     * Sets the options which configures a new {@link Channel} and its child
     * {@link Channel}s.  To set the options of a child {@link Channel}, prefixed
     * <tt>"child."</tt> to the option name (e.g. <tt>"child.keepAlive"</tt>).
     */
    T& setOptions(const ChannelOption::Options& options);

    /**
     * Sets an option with the specified key and value.  If there's already
     * an option with the same key, it is replaced with the new value.  If the
     * specified value is <tt>empty boost::any</tt>, an existing option with
     * the specified key is removed.  To set the option value of a child
     * {@link Channel}, prepend <tt>"child."</tt> to the option name
     * (e.g. <tt>"child.keepAlive"</tt>).
     *
     * @param key    the option name
     * @param value  the option value
     */
    T& setOption(const ChannelOption& option,
                                         const ChannelOption::Variant& value);

    /**
     * Shutdown the {@link AbstractBootstrap} and the {@link EventLoopGroup} which is
     * used by it. Only call this if you don't share the {@link EventLoopGroup}
     * between different {@link AbstractBootstrap}'s.
     */
    virtual void shutdown() {
        if (eventLoopPool_) {
            eventLoopPool_->stop();
            eventLoopPool_->waitForStop();
        }
    }

protected:
    /**
     * Creates a new instance with no {@link ChannelFactory} set.
     * {@link #setFactory(ChannelFactory)} must be called at once before any
     * I/O operation is requested.
     */
    AbstractBootstrap() {}
    
    AbstractBootstrap(const EventLoopPoolPtr& pool)
        : eventLoopPool_(pool) {
    }

private:
    T& castThis() {
        return *(static_cast<T*>(this));
    }

private:
    SocketAddress localAddress_;
    EventLoopPoolPtr eventLoopPool_;
    ChannelOption::Options options_;
};

template<typename T> inline
const SocketAddress& AbstractBootstrap<T>::localAddress() const {
    return localAddress_;
}

template<typename T> inline
T& AbstractBootstrap<T>::setLocalAddress(const SocketAddress& address) {
    localAddress_ = address;
    return castThis();
}

template<typename T> inline
T& AbstractBootstrap<T>::setLocalAddress(int port) {
    localAddress_ = SocketAddress(port);
    return castThis();
}

template<typename T> inline
T& AbstractBootstrap<T>::setLocalAddress(const std::string& host, int port) {
    localAddress_ = SocketAddress(host, port);
    return castThis();
}

template<typename T> inline
const EventLoopPoolPtr& AbstractBootstrap<T>::eventLoopPool() const {
    return eventLoopPool_;
}

template<typename T> inline
T& AbstractBootstrap<T>::setEventLoopPool(const EventLoopPoolPtr& pool) {
    eventLoopPool_ = pool;
    return castThis();
}

template<typename T> inline
ChannelOption::Options& AbstractBootstrap<T>::options() {
    return options_;
}

template<typename T> inline
const ChannelOption::Options& AbstractBootstrap<T>::options() const {
    return options_;
}

template<typename T> inline
ChannelOption::Variant AbstractBootstrap<T>::getOption(const ChannelOption& option) const {
    ChannelOption::Options::const_iterator itr = options.find(option);

    if (itr == options.end()) {
        LOG_WARN << "can not get the option of " << option.name();
        return ChannelOption::Variant();
    }

    return itr->second;
}

template<typename T> inline
T& AbstractBootstrap<T>::setOptions(const ChannelOption::Options& options) {
    LOG_INFO << "set options using map, will reset the original options.";
    this->options = options;

    return castThis();
}

template<typename T> inline
T& AbstractBootstrap<T>::setOption(const ChannelOption& option,
        const ChannelOption::Variant& value) {
    if (value.empty()) {
        options_.erase(option);
        LOG_WARN << "setOption, the key ("
                 << option.name()
                 << ") is empty value, remove from the options.";
    }
    else {
        LOG_DEBUG << "set Option, the key is " << option.name();
        options_.insert(std::make_pair(option, value));
    }

    return castThis();
}

}
}

#endif //#if !defined(CETTY_BOOTSTRAP_ABSTRACTBOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
