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
#include <boost/scoped_ptr.hpp>
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
     * return the local address which is bind to.
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
     * The {@link InetAddress} which is used to bind the local "end" to.
     */
    T& setLocalAddress(const InetAddress& localAddress);


    /**
     * Return the {@link EventLoopPool} which is belong to.
     */
    const EventLoopPoolPtr& eventLoopPool() const;

    /**
     * The {@link EventLoopPool} which is used to handle all the events for the to-be-creates
     * {@link Channel}
     */
    virtual T& setEventLoopPool(const EventLoopPoolPtr& pool);

    /**
     * Returns the options which configures a new {@link Channel}.
     */
    ChannelOptions& options();

    /**
     * Returns the options which configures a new {@link Channel}.
     */
    const ChannelOptions& options() const;

    /**
     * Returns the value of the option with the specified key.
     *
     * @param key  the option name
     *
     * @return the option value if the option is found.
     *         <tt>an empty ChannelOption::Variant</tt> otherwise.
     *
     * @remark check the {@link ChannelOption::Variant} is empty, using ChannelOption::empty()
     */
    ChannelOption::Variant option(const ChannelOption& key) const;

    /**
     * Sets the options which configures a new {@link Channel}.
     */
    T& setOptions(const ChannelOptions& options);

    /**
     * Allow to specify a {@link ChannelOption} which is used for the
     * {@link Channel} instances once they got created.
     * Use {@code ChannelOption::EMPTY_VALUE} to remove a previous set {@link ChannelOption}.
     *
     * @param key      the option name
     * @param value    the option value
     */
    T& setOption(const ChannelOption& key,
                 const ChannelOption::Variant& value);

    /**
     * the {@link ChannelHandler} to use for serving the requests.
     */
    template<typename Handler>
    T& setHandler(
        const typename ChannelHandlerWrapper<Handler>::HandlerPtr& handler) {
        handler_.reset(
            new typename ChannelHandlerWrapper<Handler>::Handler::Context(
                "_user",
                handler));

        return castThis();
    }

    /**
     * return the channels which has created.
     */
    Channels& channels();
    const Channels& channels() const;

    /**
     * Shutdown the {@link Bootstrap} and the {@link EventLoopPool} which is
     * used by it. Only call this if you don't share the {@link EventLoopPool}
     * between different {@link Bootstrap}'s.
     */
    virtual void shutdown() = 0;

    /**
     * Waiting for all the {@link channel}s in the {@link Bootstrap} to close.
     */
    virtual void waitingForExit() = 0;

protected:
    Bootstrap() {
    }

    Bootstrap(const EventLoopPoolPtr& pool)
        : eventLoopPool_(pool) {
    }

    ChannelHandlerContext* handler();

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
    boost::scoped_ptr<ChannelHandlerContext> handler_;

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
    if (pool) {
        eventLoopPool_ = pool;
    }
    else {
        LOG_WARN << "setting the NULL EventLoopPool, skip it";
    }

    return castThis();
}

template<typename T> inline
ChannelOptions& Bootstrap<T>::options() {
    return options_;
}

template<typename T> inline
const ChannelOptions& Bootstrap<T>::options() const {
    return options_;
}

template<typename T> inline
ChannelOption::Variant Bootstrap<T>::option(const ChannelOption& key) const {
    ChannelOptions::ConstIterator itr = options_.find(key);

    if (itr == options_.end()) {
        LOG_WARN << "can not get the option of " << key.name();
        return ChannelOption::Variant();
    }

    return itr->second;
}

template<typename T> inline
T& Bootstrap<T>::setOptions(const ChannelOptions& options) {
    LOG_INFO << "set options using map, will reset the original options.";
    options_ = options;

    return castThis();
}

template<typename T> inline
T& Bootstrap<T>::setOption(const ChannelOption& key,
                           const ChannelOption::Variant& value) {
    options_.setOption(key, value);
    return castThis();
}

template<typename T> inline
ChannelHandlerContext* Bootstrap<T>::handler() {
    return handler_.get();
}

template<typename T> inline
void Bootstrap<T>::clearChannels() {
    channels_.clear();
}

template<typename T> inline
void cetty::bootstrap::Bootstrap<T>::removeChannel(int id) {
    channels_.erase(id);
}

template<typename T> inline
void cetty::bootstrap::Bootstrap<T>::insertChannel(int id, const ChannelPtr& channel) {
    channels_.insert(std::make_pair(id, channel));
}

}
}

#endif //#if !defined(CETTY_BOOTSTRAP_BOOTSTRAP_H)

// Local Variables:
// mode: c++
// End:
