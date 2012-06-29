#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEPTR_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEPTR_H

#include <boost/intrusive_ptr.hpp>

namespace cetty {
namespace protobuf {
namespace service {

class ProtobufService;
typedef boost::intrusive_ptr<ProtobufService> ProtobufServicePtr;

}
}
}


#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEPTR_H)

// Local Variables:
// mode: c++
// End:
