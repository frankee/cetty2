#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFCODEC_H)
#define CETTY_SHUTTLE_PROTOBUF_PROTOBUFCODEC_H

#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/handler/codec/MessageToMessageCodec.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessagePtr.h>

namespace cetty {
namespace shuttle {
namespace protobuf {

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::handler::codec;

class ProtobufProxyCodec : private boost::noncopyable {
public:
    typedef MessageToMessageCodec<ProtobufProxyCodec,
        ChannelBufferPtr,
        ProtobufProxyMessagePtr,
        ProtobufProxyMessagePtr,
        ChannelBufferPtr> Codec;

    typedef Codec::Context Context;
    typedef Codec::Handler Handler;
    typedef Codec::HandlerPtr HandlerPtr;

public:
    ProtobufProxyCodec()
        : codec_(boost::bind(&ProtobufProxyCodec::decode, this, _1, _2),
        boost::bind(&ProtobufProxyCodec::encode, this, _1, _2)) {
    }

    void registerTo(Context& ctx) {
        codec_.registerTo(ctx);
    }

private:
    ProtobufProxyMessagePtr decode(ChannelHandlerContext& ctx,
        const ChannelBufferPtr& msg);

    ChannelBufferPtr encode(ChannelHandlerContext& ctx,
        const ProtobufProxyMessagePtr& msg);

private:
    Codec codec_;
};

}
}
}

#endif //#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFCODEC_H)

// Local Variables:
// mode: c++
// End:
