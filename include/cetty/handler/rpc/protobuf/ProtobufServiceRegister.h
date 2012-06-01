#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEREGISTER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEREGISTER_H

#include <map>
#include <string>
#include <boost/smart_ptr.hpp>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/handler/rpc/protobuf/ProtobufServicePtr.h>

namespace google {
namespace protobuf {
class Service;
class MethodDescriptor;
}
}

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

class ProtobufServiceRegister {
public:
    typedef std::map<std::string, ProtobufServicePtr> ServiceMapType;
    typedef google::protobuf::MethodDescriptor MethodType;

public:
    int registerService(ProtobufServicePtr& service);
    void unregisterService(const std::string& name);

    ProtobufServicePtr& getService(const std::string& name);

    const MethodType* getMethod(const ProtobufServicePtr& service, const std::string& method);

private:
    static ProtobufServicePtr nullService;

private:
    ServiceMapType serviceMap;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEREGISTER_H)

// Local Variables:
// mode: c++
// End:
