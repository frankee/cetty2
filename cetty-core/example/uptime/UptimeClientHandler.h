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
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/thread/thread_time.hpp>

#include "cetty/channel/Channel.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/bootstrap/ClientBootstrap.h"

#include "cetty/util/Exception.h"
#include "cetty/util/TimeUnit.h"
#include "cetty/util/Timer.h"

using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::util;

extern const int RECONNECT_DELAY;

/**
 * Keep reconnecting to the server while printing out the current uptime and
 * connection attempt status.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2189 $, $Date: 2010-02-19 18:02:57 +0900 (Fri, 19 Feb 2010) $
 */
class UptimeClientHandler : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    typedef boost::posix_time::ptime time_type;

    UptimeClientHandler(ClientBootstrap& bootstrap) : bootstrap(bootstrap) {
    }

    virtual ~UptimeClientHandler() {}

    const std::string& getRemoteAddress() {
        if (socketAddressStr.empty()) {
            socketAddressStr =
                bootstrap.getTypedOption<SocketAddress>("remoteAddress")->toString();
        }
        return socketAddressStr;
    }

    virtual void channelDisconnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        println("Disconnected from: " + getRemoteAddress());
    }

    virtual void channelClosed(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        println("Sleeping for: " + Integer::toString(RECONNECT_DELAY) + "s");
        if (!timer) {
            timer = TimerFactory::getFactory().getTimer(e.getChannel());
        }

        timer->newTimeout(boost::bind(
            &UptimeClientHandler::handleReConnection,
            this,
            _1,
            boost::ref(bootstrap)),
            RECONNECT_DELAY, TimeUnit::SECONDS);
    }

    void handleReConnection(Timeout& timeout, ClientBootstrap& bootstrap) {
        println("Reconnecting to: " + getRemoteAddress());
        bootstrap.connect();
    }

    virtual void channelConnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        if (startTime.is_not_a_date_time()) {
            startTime = boost::get_system_time();
        }

        if (!timer) {
            timer = TimerFactory::getFactory().getTimer(e.getChannel());
        }

        println("Connected to: " + getRemoteAddress());
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        startTime = boost::posix_time::not_a_date_time;
        
        //const Exception& cause = e.getCause();
        //println("Failed to connect: " + cause.getMessage());

        ctx.getChannel().close();
    }

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }
    virtual std::string toString() const { return "UptimeClientHandler"; }

private:
    void println(const std::string& msg) {
        if (startTime.is_not_a_date_time()) {
            printf("[SERVER IS DOWN] %s\n", msg.c_str());
        }
        else {
            boost::posix_time::time_duration duration = boost::get_system_time() - startTime;
            printf("[UPTIME: %5d] %s\n", duration.total_seconds(), msg.c_str());
        }
    }

    void println(const std::string& msg, const std::string& arg1) {
        if (startTime.is_not_a_date_time()) {
            printf("[SERVER IS DOWN] %s%s\n", msg.c_str(), arg1.c_str());
        }
        else {
            boost::posix_time::time_duration duration = boost::get_system_time() - startTime;
            printf("[UPTIME: %5d] %s%s\n", duration.total_seconds(), msg.c_str(), arg1.c_str());
        }
    }

    void println(const std::string& msg, const std::string& arg1, const std::string& arg2) {
        if (startTime.is_not_a_date_time()) {
            printf("[SERVER IS DOWN] %s%s%s\n", msg.c_str(), arg1.c_str(), arg2.c_str());
        }
        else {
            boost::posix_time::time_duration duration = boost::get_system_time() - startTime;
            printf("[UPTIME: %5d] %s%s%s\n", duration.total_seconds(), msg.c_str(), arg1.c_str(), arg2.c_str());
        }
    }

private:
    std::string socketAddressStr;

    ClientBootstrap& bootstrap;
    TimerPtr timer;
    time_type startTime;
};
