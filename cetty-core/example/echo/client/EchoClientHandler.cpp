#include "EchoClientHandler.h"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/thread/thread_time.hpp>

#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelPipeline.h>

#include <cetty/logging/LoggerHelper.h>

void EchoClientHandler::messageUpdated(ChannelHandlerContext& ctx) {
    // Send back the received message to the remote peer.
    const ChannelBufferPtr& buffer = container_->getMessages();

    if (buffer) {
        int readableBytes = buffer->readableBytes();

        ChannelBufferPtr tmp = buffer->readBytes();

        transfer_->write(tmp, ctx.newSucceededFuture());

        printf("received message at %s with %dBytes.\n",
               boost::posix_time::to_simple_string(boost::get_system_time()).c_str(),
               readableBytes);
    }
}

void EchoClientHandler::channelActive(ChannelHandlerContext& ctx) {
    // Send the first message.  Server will not send anything here
    // because the firstMessage's capacity is 0.
    transfer_->write(firstMessage, ctx.newSucceededFuture());

    //outboundTransfer.write(firstMessage, ctx.channel()->newSucceededFuture());
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
