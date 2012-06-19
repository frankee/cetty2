#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEREGISTER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEREGISTER_H

#include <map>
#include <string>
#include <boost/smart_ptr.hpp>
#include "cetty/util/ReferenceCounter.h"
#include "cetty/protobuf/ProtobufServicePtr.h"

namespace google {
namespace protobuf {
class Service;
class MethodDescriptor;
}
}

namespace cetty {
namespace protobuf {

class ProtobufServiceRegister {
public:
    typedef google::protobuf::MethodDescriptor MethodDescriptor;

private:
    typedef std::map<std::string, ProtobufServicePtr> ServiceMap;

public:
    static ProtobufServiceRegister& instance();

public:
    int  registerService(ProtobufServicePtr& service);
    void unregisterService(const std::string& name);

    ProtobufServicePtr& getService(const std::string& name) const;
    int getRegisteredServices(std::vector<std::string>* names) const;

    const MethodDescriptor* getMethodDescriptor(const ProtobufServicePtr& service,
            const std::string& method) const;

    const google::protobuf::Message* getRequestPrototype(const std::string& service, const std::string& method) const {
        ServiceMap::const_iterator itr = serviceMap.find(service);

        if (itr != serviceMap.end()) {
            return getRequestPrototype(itr->second, method);
        }

        return NULL;
    }
    const google::protobuf::Message* getResponsePrototype(const std::string& service, const std::string& method) const {
        ServiceMap::const_iterator itr = serviceMap.find(service);

        if (itr != serviceMap.end()) {
            return getResponsePrototype(itr->second, method);
        }

        return NULL;
    }

    const google::protobuf::Message* getRequestPrototype(const ProtobufServicePtr& service, const std::string& method) const {
        if (!service || method.empty()) {
            return NULL;
        }

        const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();

        if (desc) {
            const MethodDescriptor* methodDescriptor = desc->FindMethodByName(method);

            if (methodDescriptor) {
                return service->GetRequestPrototype(methodDescriptor);
            }
        }

        return NULL;
    }
    const google::protobuf::Message* getRequestPrototype(const ProtobufServicePtr& service, const std::string& method) const {
        if (!service) {
            return NULL;
        }

        const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();

        if (desc) {
            const MethodDescriptor* methodDescriptor = desc->FindMethodByName(method);

            if (methodDescriptor) {
                return service->GetResponsePrototype(methodDescriptor);
            }
        }

        return NULL;
    }


private:
    ProtobufServiceRegister(const ProtobufServiceRegister&);
    ProtobufServiceRegister& operator=(const ProtobufServiceRegister&);

private:
    static ProtobufServicePtr nullService;

private:
    ServiceMap serviceMap;
};

}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFSERVICEREGISTER_H)
