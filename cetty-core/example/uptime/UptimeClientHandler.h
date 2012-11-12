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
#include <boost/bind.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundBufferHandlerAdapter.h>

#include <cetty/bootstrap/ClientBootstrap.h>

#include <cetty/handler/timeout/IdleStateEvent.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>

using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::util;
using namespace cetty::handler::timeout;

extern const int RECONNECT_DELAY;

/**
 * Keep reconnecting to the server while printing out the current uptime and
 * connection attempt status.
 */
class UptimeClientHandler : public ChannelInboundBufferHandlerAdapter<> {
public:
    typedef boost::posix_time::ptime Time;

public:
    UptimeClientHandler(ClientBootstrap& bootstrap)
        : bootstrap(bootstrap) {
    }

    virtual ~UptimeClientHandler() {}

    virtual void channelActive(ChannelHandlerContext& ctx) {
        if (startTime.is_not_a_date_time()) {
            startTime = boost::get_system_time();
        }

        println(StringUtil::printf("Connected to: %s",
                                      getRemoteAddress().c_str()));
    }

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        // Discard received data
        getInboundChannelBuffer()->clear();
    }

    virtual void userEventTriggered(ChannelHandlerContext& ctx,
                                    const boost::any& evt) {
        const IdleStateEvent* e = boost::any_cast<IdleStateEvent>(&evt);

        if (e && e->getState() == IdleState::READER_IDLE) {
            // The connection was OK but there was no traffic for last period.
            printf("Disconnecting due to no inbound traffic");
            ctx.close();
        }
    }

    virtual void channelInactive(ChannelHandlerContext& ctx) {
        printf(StringUtil::printf("Sleeping for: %d s", RECONNECT_DELAY).c_str());

        timeout = ctx.getEventLoop()->runAfter(RECONNECT_DELAY * 1000,
                                               boost::bind(
                                                       &UptimeClientHandler::handleReConnection,
                                                       this,
                                                       boost::ref(bootstrap)));
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ChannelException& e) {
        startTime = boost::posix_time::not_a_date_time;

        //const Exception& cause = e.getCause();
        //println("Failed to connect: " + cause.getMessage());

        ctx.close();
    }

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }
    virtual std::string toString() const { return "UptimeClientHandler"; }

private:
    void handleReConnection(ClientBootstrap& bootstrap) {
        println(StringUtil::printf("Reconnecting to: %s", getRemoteAddress().c_str()));
        bootstrap.connect();
    }

    std::string getRemoteAddress() {
        return bootstrap.remoteAddress().toString();
    }

    void println(const std::string& msg) {
        if (startTime.is_not_a_date_time()) {
            printf("[SERVER IS DOWN] %s\n", msg.c_str());
        }
        else {
            boost::posix_time::time_duration duration = boost::get_system_time() - startTime;
            printf("[UPTIME: %5d] %s\n", duration.total_seconds(), msg.c_str());
        }
    }

private:
    ClientBootstrap& bootstrap;

    TimeoutPtr timeout;
    Time startTime;
};
