#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFPROXYMESSAGEPTR_H)
#define CETTY_SHUTTLE_PROTOBUF_PROTOBUFPROXYMESSAGEPTR_H

#include <boost/intrusive_ptr.hpp>

namespace cetty {
namespace shuttle {
namespace protobuf {

class ProtobufProxyMessage;
typedef boost::intrusive_ptr<ProtobufProxyMessage> ProtobufProxyMessagePtr;

}
}
}


#endif //#if !defined(CETTY_SHUTTLE_PROTOBUF_PROTOBUFPROXYMESSAGEPTR_H)

// Local Variables:
// mode: c++
// End:
