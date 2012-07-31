#include "EchoClientHandler.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/thread/thread_time.hpp>

#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/logging/InternalLoggerFactory.h>

InternalLogger* EchoClientHandler::logger =
    InternalLoggerFactory::getInstance("EchoClientHandler");

void EchoClientHandler::messageUpdated(ChannelInboundBufferHandlerContext& ctx) {
    // Send back the received message to the remote peer.
    const ChannelBufferPtr& buffer = ctx.getInboundChannelBuffer();

    if (buffer) {
        int readableBytes = buffer->readableBytes();

        ChannelBufferPtr tmp = buffer->readBytes();
        ChannelPtr channel = ctx.getChannel();

        if (intervalTime == 0) {
            channel->write(tmp);
        }
        else {
            timer->newTimeout(boost::bind(&EchoClientHandler::delaySendMessage,
                this,
                _1,
                boost::ref(*channel),
                tmp),
                intervalTime);
        }

        printf("received message from %d at %s.\n", channel->getId(), boost::posix_time::to_simple_string(boost::get_system_time()).c_str());
    }
}

void EchoClientHandler::delaySendMessage(Timeout& timeout, Channel& channel, const ChannelBufferPtr& buffer) {
    printf("send message from %d at %s.\n", channel.getId(), boost::posix_time::to_simple_string(boost::get_system_time()).c_str());
    channel.write(buffer);
}

void EchoClientHandler::channelActive(ChannelHandlerContext& ctx) {
    // Send the first message.  Server will not send anything here
    // because the firstMessage's capacity is 0.
    timer = TimerFactory::getFactory().getTimer(ctx.getChannel());
    ctx.getPipeline().write(firstMessage);
}
