///*
//先encode,然后再decode
//*/
//#include <cstdio>
//#include <cetty/protobuf/service/ProtobufServiceMessage.h>
//#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
//#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
//#include <cetty/buffer/ChannelBuffer.h>
//#include <cetty/protobuf/service/proto/service.pb.h>
//#include <cetty/channel/ChannelHandlerContext.h>
//#include <cetty/channel/NullChannel.h>
//
//#include "gtest/gtest.h"
//
//using namespace cetty::protobuf::service::handler;
//using namespace cetty::buffer;
//using namespace cetty::channel;
//
//
//ChannelMessage channelMsgGloble;
//
//
//class QuickTest : public testing::Test {
//protected:
//	// Remember that SetUp() is run immediately before a test starts.
//	// This is a good place to record the start time.
//	virtual void SetUp() {
//		start_time_ = time(NULL);
//	}
//
//	// TearDown() is invoked immediately after a test finishes.  Here we
//	// check if the test was too slow.
//	virtual void TearDown() {
//		// Gets the time when the test finishes
//		const time_t end_time = time(NULL);
//
//		// Asserts that the test took no more than ~5 seconds.  Did you
//		// know that you can use assertions in SetUp() and TearDown() as
//		// well?
//		printf("the elapse time is %d\n",end_time - start_time_);
//	}
//
//	// The UTC time (in seconds) when the test starts
//	time_t start_time_;
//};
//
//class NullChannelContext : public ChannelHandlerContext {
//public:
//    virtual ~NullChannelContext() {}
//
//    virtual const ChannelPtr& getChannel() const { return ChannelPtr(); }
//	virtual const ChannelPipelinePtr& getPipeline() const {return ChannelPipelinePtr();}
//    virtual const std::string& getName() const {return std::string();}
//    virtual const ChannelHandlerPtr& getHandler(){return ChannelHandlerPtr();}
//    virtual const ChannelUpstreamHandlerPtr& getUpstreamHandler(){return ChannelUpstreamHandlerPtr();}
//    virtual const ChannelDownstreamHandlerPtr& getDownstreamHandler(){return ChannelDownstreamHandlerPtr();}
//    virtual bool canHandleUpstream(){return true;}
//    virtual bool canHandleDownstream(){return true;}
//    virtual void sendUpstream(const ChannelEvent& e){return;}
//    virtual void sendUpstream(const MessageEvent& e){return;}
//    virtual void sendUpstream(const ChannelStateEvent& e){return;}
//    virtual void sendUpstream(const ChildChannelStateEvent& e) {return;}
//    virtual void sendUpstream(const WriteCompletionEvent& e) {return;}
//    virtual void sendUpstream(const ExceptionEvent& e) {return;}
//    virtual void sendDownstream(const ChannelEvent& e) {return;}
//    virtual void sendDownstream(const MessageEvent& e) {return;}
//    virtual void sendDownstream(const ChannelStateEvent& e) {return;}
//    virtual void* getAttachment() {return NULL;}
//    virtual const void* getAttachment()const {return NULL;}
//    virtual void setAttachment(void* attachment){return;}
//	static ChannelHandlerContext& getInstance()
//	{
//		return nullChannelContext;
//	}
//private:
//	static NullChannelContext nullChannelContext;
//};
//
//
//
//// To use a test fixture, derive a class from testing::Test.
//class ProtobufServiceMessageCodecTest :public QuickTest {
//protected:  
//	//initial a ProtobufServiceMessagePtr
//	virtual void SetUp() {
//		protoMsg->setType(REQUEST);
//		protoMsg->setId(10);
//		protoMsg->getServiceMessage().set_service("hello");
//		protoMsg->getServiceMessage().set_method("world");
//		google::protobuf::Message* msg = NULL;
//		protoMsg->setPayload(msg);
//	}
//
//	void testEncode(int count)
//	{
//		ProtobufServiceMessageEncoder encoder;
//		ChannelMessage channelMsg = encoder.encode(NullChannelContext::getInstance(), NullChannel::getInstance(),ChannelMessage(protoMsg));
//		channelMsgGloble = channelMsg;
//		ChannelBufferPtr channelbuffer = channelMsg.smartPointer<ChannelBuffer>();
//		ChannelBuffers::hexDump(channelbuffer);
//	}
//
//	void  testDecode(int count)
//	{
//		ProtobufServiceMessageDecoder decoder;
//		ChannelMessage msg = decoder.decode(NullChannelContext::getInstance(), NullChannel::getInstance(),channelMsgGloble);
//		ProtobufServiceMessagePtr protoMsg = msg.smartPointer<ProtobufServiceMessage>();
//		EXPECT_EQ(protoMsg->getServiceMessage().type(),REQUEST);
//		EXPECT_EQ(protoMsg->getService(),"hello");
//		EXPECT_EQ(protoMsg->getMethod(),"world");
//		EXPECT_EQ(protoMsg->getPayload(),NULL);
//	}
//
//	// Declares the variables your tests want to use.
//	ProtobufServiceMessagePtr protoMsg;
//};
//
//
//TEST_F(ProtobufServiceMessageCodecTest, testEncode) {
//	testEncode();
//}
//
//TEST_F(ProtobufServiceMessageCodecTest, testDecode) {
//	testDecode();
//}
//
//
//GTEST_API_ int main(int argc, char **argv) {
//	std::cout << "Running main() from gtest_main.cc\n";
//
//	if(argc<3)
//	{
//		std::cout << "please enter the param like ./benchmark encodeCount  decodeCount\n";
//	}
//
//	
//	testing::InitGoogleTest(&argc, argv);
//	RUN_ALL_TESTS();
//	getchar();
//	return 0;
//}