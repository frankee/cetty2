/*test for encode and decode functionality*/

#include <string>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/channel/VoidChannelMessage.h>
#include <cetty/channel/embedded/EmbeddedMessageChannel.h>
#include <cetty/protobuf/service/proto/service.pb.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>

#include <codec.pb.h>
#include <gtest/gtest.h>

using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;
using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::channel::embedded;

TEST(ProtobufServiceMessageCodecTest, encode)
{
	ProtobufServiceMessagePtr protoMsg = new ProtobufServiceMessage();
	protoMsg->setType(REQUEST);
	protoMsg->setId(10);
	protoMsg->mutableServiceMessage()->set_service("hello");
	protoMsg->mutableServiceMessage()->set_method("world");
	protoMsg->mutableServiceMessage()->set_error(INVALID_RESPONSE);
	
	CodecTest codecTest;
	codecTest.set_id(10);
	codecTest.set_f(20);
	codecTest.set_content("hello");

	protoMsg->setPayload( &codecTest);

    EmbeddedMessageChannel<VoidChannelMessage, ChannelBufferPtr> channel(
        new ProtobufServiceMessageEncoder);

    channel.writeOutbound(protoMsg);
	ChannelBufferPtr channelbuffer = channel.readOutbound();
    std::string content1;
    channelbuffer->readBytes(&content1);

    LOG_DEBUG << ChannelBuffers::hexDump(channelbuffer);
    LOG_DEBUG << content1;

    // using google protobuf lib
    ServiceMessage serviceMsg(protoMsg->getServiceMessage());
    protoMsg->getPayload()->SerializeToString(serviceMsg.mutable_request());
    
	std::string content2;
	serviceMsg.SerializeToString(&content2);
	LOG_DEBUG << content2;

	ASSERT_TRUE(content1.compare(content2) == 0);
}

TEST(ProtobufServiceMessageCodecTest, decode)
{
	ProtobufServiceMessagePtr protoMsg = new ProtobufServiceMessage();
	protoMsg->setType(REQUEST);
	protoMsg->setId(10);
	protoMsg->mutableServiceMessage()->set_service("hello");
	protoMsg->mutableServiceMessage()->set_method("world");
	protoMsg->mutableServiceMessage()->set_error(INVALID_RESPONSE);

	CodecTest codecTest;
	codecTest.set_id(10);
	codecTest.set_f(20);
	codecTest.set_content("hello");

	codecTest.SerializeToString(protoMsg->mutableServiceMessage()->mutable_request());

	std::string data;
	protoMsg->mutableServiceMessage()->SerializeToString(&data);
	ChannelBufferPtr channelbuffer = ChannelBuffers::wrappedBuffer(data);

    EmbeddedMessageChannel<ProtobufServiceMessagePtr, VoidChannelMessage> channel(
        new ProtobufServiceMessageDecoder);

    channel.writeInbound(channelbuffer);
    ProtobufServiceMessagePtr decodedMsg = channel.readInbound();

    ASSERT_TRUE(decodedMsg);
	
    EXPECT_EQ(decodedMsg->getServiceMessage().type(), REQUEST);
	EXPECT_EQ(decodedMsg->getId(), 10);
	ASSERT_TRUE(decodedMsg->getService().compare("hello") == 0);
	ASSERT_TRUE(decodedMsg->getMethod().compare("world") == 0);
}
