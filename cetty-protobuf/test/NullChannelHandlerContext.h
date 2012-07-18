#ifndef __NULL_CHANNEL_HANDLER_CONTEXT__
#define __NULL_CHANNEL_HANDLER_CONTEXT__
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/protobuf/service/proto/service.pb.h>

#include <cetty/channel/ChannelPipelineFwd.h>
#include <cetty/channel/DefaultChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>


#include <cetty/channel/ChannelHandlerContext.h>
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;
using namespace cetty::buffer;
using namespace cetty::channel;

namespace cetty {
namespace channel {
class ChannelEvent;
class MessageEvent;
class ExceptionEvent;
class ChannelStateEvent;
class WriteCompletionEvent;
class ChildChannelStateEvent;
}
}

class NullChannelContext : public ChannelHandlerContext {
public:
    static NullChannelContext* nullChannelContext;

    NullChannelContext() {}
    virtual ~NullChannelContext() {};

    const ChannelPtr& getChannel() const;
    const ChannelPipelinePtr& getPipeline()const;
    const std::string& getName() const;
    const ChannelHandlerPtr& getHandler()const;
    const ChannelInboundHandlerPtr& getInboundHandler()const;
    const ChannelOutboundHandlerPtr& getOutboundHandler()const;
    bool canHandleInboundMessage()const;
    bool canHandleOutboundMessage()const;
    void sendUpstream(const ChannelEvent& e);
    void sendUpstream(const MessageEvent& e);
    void sendUpstream(const ChannelStateEvent& e);
    void sendUpstream(const ChildChannelStateEvent& e);
    void sendUpstream(const WriteCompletionEvent& e);
    void sendUpstream(const ExceptionEvent& e);
    void sendDownstream(const ChannelEvent& e);
    void sendDownstream(const MessageEvent& e);
    void sendDownstream(const ChannelStateEvent& e);
    void* getAttachment();
    const void* getAttachment()const;
    void  setAttachment(void* attachment);

    static ChannelHandlerContext& getInstance() {
        if (nullChannelContext) {
            return *nullChannelContext;
        }
        else {
            nullChannelContext = new NullChannelContext();
            return *nullChannelContext;
        }
    }
};
#endif