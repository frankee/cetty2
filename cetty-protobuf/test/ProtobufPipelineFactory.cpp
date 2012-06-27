//
//#include <cetty/protobuf/test/ProtobufPipelineFactory.h>
//
//#include <cetty/channel/Channels.h>
//#include <cetty/channel/ChannelPipeline.h>
//#include <cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h>
//#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
//#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
////#include <cetty/gearman/GearmanMessageHandler.h>
//
//namespace cetty {
//namespace gearman {
//
//    using namespace cetty::channel;
//	using namespace cetty::protobuf::service::handler;
////using namespace cetty::handler::codec::frame;
////
////static const int MAX_FRAME_LENGTH = 1024*1024;
//
//cetty::channel::ChannelPipelinePtr GearmanPipelineFactory::getPipeline() {
//    ChannelPipelinePtr pipeline = Channels::pipeline();
//
//    //pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(MAX_FRAME_LENGTH, 8, 4, 0, 4));
//
//    pipeline->addLast("protobufDecoder", new ProtobufServiceMessageDecoder);
//    pipeline->addLast("protobufEncoder", new ProtobufServiceMessageEncoder);
//
//    //pipeline->addLast("gearmanHandler", new GearmanMessageHandler);
//
//    return pipeline;
//}
//
//}
//}
