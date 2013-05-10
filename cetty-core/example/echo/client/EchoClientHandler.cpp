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

        LOG_INFO << "received message at "
                 << boost::posix_time::to_simple_string(boost::get_system_time())
                 << " with " << readableBytes << " Bytes.";
    }
}

void EchoClientHandler::channelActive(ChannelHandlerContext& ctx) {
    // Send the first message.  Server will not send anything here
    // because the firstMessage's capacity is 0.
    transfer_->write(firstMessage_, ctx.newSucceededFuture());
}

EchoClientHandler::EchoClientHandler(int firstMessageSize)
    :firstMessageSize_(firstMessageSize),
     transferredBytes_(0) {
    if (firstMessageSize <= 0) {
        firstMessageSize = 8 * 1024;
    }

    firstMessage_ = Unpooled::buffer(firstMessageSize);
    BOOST_ASSERT(firstMessage_);
    int capacity = firstMessage_->writableBytes();

    for (int i = 0; i < capacity; i ++) {
        firstMessage_->writeByte(i);
    }
}
