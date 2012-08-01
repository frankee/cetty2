/*test for encode and decode functionality*/

#include <cstdio>
#include <string>
#include <iostream>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/protobuf/service/proto/service.pb.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelPipelinePtr.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/DefaultChannelPipeline.h>
#include <cetty/channel/NullChannel.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>

#include <NullChannelHandlerContext.h>
#include <codec.pb.h>
#include <gtest/gtest.h>

using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;
using namespace cetty::buffer;
using namespace cetty::channel;


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
	google::protobuf::Message* msg = &codecTest;

	protoMsg->setPayload(msg);
	ProtobufServiceMessageEncoder encoder;

	UserEvent channelMsg = encoder.encode(NullChannelContext::getInstance(),
		NullChannel::getInstance(), UserEvent(protoMsg));

	ChannelBufferPtr channelbuffer = channelMsg.smartPointer<ChannelBuffer>();
	
	std::string data = ChannelBuffers::hexDump(channelbuffer);
	std::cout<<data<<std::endl;
	std::string content1;
	channelbuffer->readBytes(&content1);
	std::cout<<content1<<std::endl;

	ServiceMessage protoMessage;
	protoMessage.set_type(REQUEST);
	protoMessage.set_id(10);
	protoMessage.set_service("hello");
	protoMessage.set_method("world");
	protoMessage.set_error(INVALID_RESPONSE);
	CodecTest codeMsg2;
	codeMsg2.set_id(10);
	codeMsg2.set_f(20);
	codeMsg2.set_content("hello");
	std::string* request = protoMessage.mutable_request();
	codeMsg2.SerializeToString(request);

	std::string content2;
	protoMessage.SerializeToString(&content2);
	std::cout<<content2<<std::endl;

	ASSERT_TRUE(content1.compare(content2) == 0);
}


TEST(ProtobufServiceMessageCodecTest2,decode)
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
	
	//google::protobuf::Message* msg = &codecTest;
	//protoMsg->setPayload(msg);
	//ProtobufServiceMessageEncoder encoder;
	//ChannelMessage channelMsg = encoder.encode(NullChannelContext::getInstance(),
	//	NullChannel::getInstance(), ChannelMessage(protoMsg));
	//ChannelBufferPtr channelbuffer = channelMsg.smartPointer<ChannelBuffer>();


	ProtobufServiceMessageDecoder decoder;
	UserEvent channelMsg2 = decoder.decode(NullChannelContext::getInstance(),
		NullChannel::getInstance(), UserEvent(channelbuffer));

	ProtobufServiceMessagePtr decodedMsg = channelMsg2.smartPointer<ProtobufServiceMessage>();
	if(decodedMsg)
	{
		EXPECT_EQ(decodedMsg->getServiceMessage().type(),REQUEST);
		EXPECT_EQ(decodedMsg->getId(),10);
		ASSERT_TRUE(decodedMsg->getService().compare("hello")==0);
		ASSERT_TRUE(decodedMsg->getMethod().compare("world")==0);

		/*typedef ::cetty::util::BarePointer<CodecTest> CodecTestPtr;
		((CodecTestPtr)(decodedMsg->getPayload()))->id();*/
/*
		EXPECT_EQ((CodecTest*)(decodedMsg->getPayload())->id(),10);
		EXPECT_EQ((CodecTest*)(decodedMsg->getPayload())->f(),20)*/;

	}
}
//GTEST_API_
GTEST_API_ int main(int argc, char **argv) {
	std::cout << "Running main() from gtest_main.cc\n";

	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	getchar();
	return 0;
}
