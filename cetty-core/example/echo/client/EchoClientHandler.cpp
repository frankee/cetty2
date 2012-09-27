#include "EchoClientHandler.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/thread/thread_time.hpp>

#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>

#include <cetty/logging/LoggerHelper.h>

void EchoClientHandler::messageUpdated(ChannelHandlerContext& ctx) {
    // Send back the received message to the remote peer.
    const ChannelBufferPtr& buffer = getInboundChannelBuffer();

    if (buffer) {
        int readableBytes = buffer->readableBytes();

        ChannelBufferPtr tmp = buffer->readBytes();
        ChannelPtr channel = ctx.getChannel();

        outboundTransfer.write(tmp, ctx.getChannel()->newSucceededFuture());
        //channel->write(tmp);
        printf("received message from %d at %s with %dBytes.\n",
               channel->getId(),
               boost::posix_time::to_simple_string(boost::get_system_time()).c_str(),
               readableBytes);
    }
}

void EchoClientHandler::channelActive(ChannelHandlerContext& ctx) {
    // Send the first message.  Server will not send anything here
    // because the firstMessage's capacity is 0.

    outboundTransfer.write(firstMessage, ctx.getChannel()->newSucceededFuture());
    //ctx.getChannel()->write(firstMessage);
    //ctx.getPipeline().write(firstMessage);
}

EchoClientHandler::EchoClientHandler(int firstMessageSize)
    :firstMessageSize(firstMessageSize), 
     transferredBytes(0) {
    if (firstMessageSize <= 0) {
        throw InvalidArgumentException("firstMessageSize must > 0.");
    }

    firstMessage = Unpooled::buffer(firstMessageSize);
    BOOST_ASSERT(firstMessage);
    int capacity = firstMessage->writableBytes();

    for (int i = 0; i < capacity; i ++) {
        firstMessage->writeByte(i);
    }
}

void EchoClientHandler::exceptionCaught(ChannelHandlerContext& ctx, const ChannelException& e) {
    // Close the connection when an exception is raised.
    LOG_WARN_E(e) << "Unexpected exception from downstream.";
    ctx.close();
}

cetty::channel::ChannelHandlerPtr EchoClientHandler::clone() {
    return new EchoClientHandler(firstMessageSize);
}

std::string EchoClientHandler::toString() const {
    return "EchoClientHandler";
}
