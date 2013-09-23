#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFPROXYMESSAGE_H)
#define CETTY_SHUTTLE_PROTOBUF_PROTOBUFPROXYMESSAGE_H

#include <cetty/util/ReferenceCounter.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessagePtr.h>

namespace cetty {
    namespace protobuf {
        namespace service {
            class ServiceMessage;
        }
    }
}

namespace cetty {
namespace shuttle {
namespace protobuf {

using namespace cetty::buffer;
using namespace cetty::protobuf::service;

class ProtobufProxyMessage : public cetty::util::ReferenceCounter<ProtobufProxyMessage> {
public:
    ProtobufProxyMessage();
    virtual ~ProtobufProxyMessage();

    ServiceMessage& message();

    const ChannelBufferPtr& buffer() const;
    void setBuffer(const ChannelBufferPtr& buffer);

private:
    ServiceMessage* message_;
    ChannelBufferPtr buffer_;
};

inline
ServiceMessage& ProtobufProxyMessage::message() {
    return *message_;
}

inline
const ChannelBufferPtr& ProtobufProxyMessage::buffer() const {
    return buffer_;
}

inline
void ProtobufProxyMessage::setBuffer(const ChannelBufferPtr& buffer) {
    buffer_ = buffer;
}

}
}
}

#endif //#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFPROXYMESSAGE_H)

// Local Variables:
// mode: c++
// End:
