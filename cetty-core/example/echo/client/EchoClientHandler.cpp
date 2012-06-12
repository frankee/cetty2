#include "EchoClientHandler.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include "boost/date_time/posix_time/time_formatters.hpp"
#include <boost/thread/thread_time.hpp>

#include "cetty/channel/ChannelMessage.h"
#include <cetty/channel/MessageEvent.h>
#include "cetty/channel/ChannelConfig.h"
#include "cetty/channel/Channels.h"

#include "cetty/channel/socket/asio/AsioClientSocketChannel.h"

#include "cetty/buffer/ChannelBufferFactory.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"

#include "cetty/logging/InternalLoggerFactory.h"

using namespace cetty::channel::socket::asio;

InternalLogger* EchoClientHandler::logger =
    InternalLoggerFactory::getInstance("EchoClientHandler");


void EchoClientHandler::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    // Send back the received message to the remote peer.
    //transferredBytes.addAndGet(((ChannelBuffer) e.getMessage()).readableBytes());

    const ChannelBufferPtr& buffer = e.getMessage().value<ChannelBufferPtr>();

    if (buffer) {
        int readableBytes = buffer->readableBytes();

        ChannelBufferPtr tmp = buffer->readBytes();
        ChannelPtr channel = e.getChannel();

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
