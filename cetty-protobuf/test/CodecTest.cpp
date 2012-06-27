/*test for encode and decode functionality*/

#include <cstdio>
#include <string>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/protobuf/service/proto/service.pb.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelPipelineFwd.h>
#include <cetty/channel/DefaultChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/NullChannel.h>
#include <cetty/protobuf/test/NullChannelHandlerContext.h>
#include <gtest/gtest.h>

using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;
using namespace cetty::buffer;
using namespace cetty::channel;

ChannelMessage channelMsgGloble;

// To use a test fixture, derive a class from testing::Test.
class ProtobufServiceMessageCodecTest : public testing::Test {
protected:  
	//initial a ProtobufServiceMessagePtr
	virtual void SetUp() {
		protoMsg->setType(REQUEST);
		protoMsg->setId(10);
		protoMsg->mutableServiceMessage()->set_service("hello");
		protoMsg->mutableServiceMessage()->set_method("world");
		google::protobuf::Message* msg = NULL;
		protoMsg->setPayload(msg);
	}

	void testEncode()
	{
		ProtobufServiceMessageEncoder encoder;
		ChannelMessage channelMsg = encoder.encode(*(NullChannelContext::getInstance()), NullChannel::getInstance(),ChannelMessage(protoMsg));
		channelMsgGloble = channelMsg;
		ChannelBufferPtr channelbuffer = channelMsg.smartPointer<ChannelBuffer>();
		ChannelBuffers::hexDump(channelbuffer);
	}

	void  testDecode()
	{
		ProtobufServiceMessageDecoder decoder;
		ChannelMessage msg = decoder.decode(*(NullChannelContext::getInstance()), NullChannel::getInstance(),channelMsgGloble);
		ProtobufServiceMessagePtr protoMsg = msg.smartPointer<ProtobufServiceMessage>();
		EXPECT_EQ((int)(protoMsg->getServiceMessage().type()),REQUEST);
		EXPECT_STREQ(protoMsg->getService().c_str(),"hello");
		EXPECT_STREQ(protoMsg->getMethod().c_str(),"world");
		EXPECT_TRUE(protoMsg->getPayload(),NULL);
	}

	// Declares the variables your tests want to use.
	ProtobufServiceMessagePtr protoMsg;
};

TEST_F(ProtobufServiceMessageCodecTest, testEncode) {
	testEncode();
}

TEST_F(ProtobufServiceMessageCodecTest, testDecode) {
	testDecode();
}

//GTEST_API_
GTEST_API_ int main(int argc, char **argv) {
	std::cout << "Running main() from gtest_main.cc\n";

	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	getchar();
	return 0;
}

