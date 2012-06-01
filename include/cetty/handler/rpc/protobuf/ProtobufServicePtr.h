#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEPTR_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEPTR_H

namespace google {
namespace protobuf {
class Service;
}
}

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

typedef boost::shared_ptr<google::protobuf::Service> ProtobufServicePtr;

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEPTR_H)

// Local Variables:
// mode: c++
// End:

