#if !defined(CETTY_BOOTSTRAP_BOOTSTRAP_H)
#define CETTY_BOOTSTRAP_BOOTSTRAP_H

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
#include <map>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/util/ReferenceCounter.h>

#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/InetAddress.h>
#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/ChannelOptions.h>

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
class Bootstrap : public cetty::util::ReferenceCounter<T> {
public:
    typedef std::map<int, ChannelPtr> Channels;

public:
    virtual ~Bootstrap() {}

    /**
     *
     */
    const InetAddress& localAddress() const;

    /**
     * See {@link #setLocalAddress(const InetAddress&)}
     */
    T& setLocalAddress(int port);

    /**
     * See {@link #localAddress(const InetAddress&)}
     */
    T& setLocalAddress(const std::string& host, int port);

    /**
     *
     */
    T& setLocalAddress(const InetAddress& localAddress);

    /**
     *
     */
    const EventLoopPoolPtr& eventLoopPool() const;

    /**
     *
     */
    T& setEventLoopPool(const EventLoopPoolPtr& pool);

    /**
     * Returns the options which configures a new {@link Channel} and its
     * child {@link Channel}s.  The names of the child {@link Channel} options
     * are prefixed with <tt>"child."</tt> (e.g. <tt>"child.keepAlive"</tt>).
     */
    ChannelOptions& channelOptions();

    /**
     * Returns the options which configures a new {@link Channel} and its
     * child {@link Channel}s.  The names of the child {@link Channel} options
     * are prefixed with <tt>"child."</tt> (e.g. <tt>"child.keepAlive"</tt>).
     */
    const ChannelOptions& channelOptions() const;

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
    ChannelOption::Variant channelOption(const ChannelOption& option) const;

    /**
     * Sets the options which configures a new {@link Channel} and its child
     * {@link Channel}s.  To set the options of a child {@link Channel}, prefixed
     * <tt>"child."</tt> to the option name (e.g. <tt>"child.keepAlive"</tt>).
     */
    T& setChannelOptions(const ChannelOptions& options);

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
    T& setChannelOption(const ChannelOption& option,
                        const ChannelOption::Variant& value);

    Channels& channels();
    const Channels& channels() const;

    /**
     * Shutdown the {@link AbstractBootstrap} and the {@link EventLoopGroup} which is
     * used by it. Only call this if you don't share the {@link EventLoopGroup}
     * between different {@link AbstractBootstrap}'s.
     */
    virtual void shutdown() = 0;

    virtual void waitingForExit() = 0;

protected:
    /**
     * Creates a new instance with no {@link ChannelFactory} set.
     * {@link #setFactory(ChannelFactory)} must be called at once before any
     * I/O operation is requested.
     */
    Bootstrap() {}

    Bootstrap(const EventLoopPoolPtr& pool)
        : eventLoopPool_(pool) {
    }

    void insertChannel(int id, const ChannelPtr& channel);
    void removeChannel(int id);
    void clearChannels();

private:
    T& castThis() {
        return *(static_cast<T*>(this));
    }

private:
    ChannelOptions options_;
    InetAddress    localAddress_;
    EventLoopPoolPtr eventLoopPool_;

    Channels channels_;
};

template<typename T> inline
typename Bootstrap<T>::Channels& Bootstrap<T>::channels() {
    return channels_;
}

template<typename T> inline
const typename Bootstrap<T>::Channels& Bootstrap<T>::channels() const {
    return channels_;
}

template<typename T> inline
const InetAddress& Bootstrap<T>::localAddress() const {
    return localAddress_;
}

template<typename T> inline
T& Bootstrap<T>::setLocalAddress(const InetAddress& address) {
    localAddress_ = address;
    return castThis();
}

template<typename T> inline
T& Bootstrap<T>::setLocalAddress(int port) {
    localAddress_ = InetAddress(port);
    return castThis();
}

template<typename T> inline
T& Bootstrap<T>::setLocalAddress(const std::string& host, int port) {
    localAddress_ = InetAddress(host, port);
    return castThis();
}

template<typename T> inline
const EventLoopPoolPtr& Bootstrap<T>::eventLoopPool() const {
    return eventLoopPool_;
}

template<typename T> inline
T& Bootstrap<T>::setEventLoopPool(const EventLoopPoolPtr& pool) {
    eventLoopPool_ = pool;
    return castThis();
}

template<typename T> inline
ChannelOptions& Bootstrap<T>::channelOptions() {
    return options_;
}

template<typename T> inline
const ChannelOptions& Bootstrap<T>::channelOptions() const {
    return options_;
}

template<typename T> inline
ChannelOption::Variant Bootstrap<T>::channelOption(const ChannelOption& option) const {
    ChannelOptions::ConstIterator itr = options_.find(option);

    if (itr == options_.end()) {
        LOG_WARN << "can not get the option of " << option.name();
        return ChannelOption::Variant();
    }

    return itr->second;
}

template<typename T> inline
T& Bootstrap<T>::setChannelOptions(const ChannelOptions& options) {
    LOG_INFO << "set options using map, will reset the original options.";
    options_ = options;

    return castThis();
}

template<typename T> inline
T& Bootstrap<T>::setChannelOption(const ChannelOption& option,
                                  const ChannelOption::Variant& value) {
    options_.setOption(option, value);
    return castThis();
}

template<typename T>
void Bootstrap<T>::clearChannels() {
    channels_.clear();
}


template<typename T>
void cetty::bootstrap::Bootstrap<T>::removeChannel(int id) {
    channels_.erase(id);
}


template<typename T>
void cetty::bootstrap::Bootstrap<T>::insertChannel(int id, const ChannelPtr& channel) {
    channels_.insert(std::make_pair(id, channel));
}

}
}

#endif //#if !defined(CETTY_BOOTSTRAP_BOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
