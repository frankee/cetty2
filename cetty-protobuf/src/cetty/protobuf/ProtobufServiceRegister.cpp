#include "cetty/protobuf/ProtobufServiceRegister.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

ProtobufServicePtr ProtobufServiceRegister::nullService;

int ProtobufServiceRegister::registerService(ProtobufServicePtr& service) {
    if (!service) {
        printf("the name or the service SHOULD NOT be NULL.\n");
        return -1;
    }

    std::string serviceName = service->GetDescriptor()->full_name();

    ServiceMapType::const_iterator itr = serviceMap.find(serviceName);

    if (itr != serviceMap.end()) {
        printf("the name of the service already registered, will be update.\n");
        serviceMap.erase(itr);
    }

    serviceMap[serviceName] = service;
    return 0;
}

void ProtobufServiceRegister::unregisterService(const std::string& name) {
    ServiceMapType::const_iterator itr = serviceMap.find(name);

    if (itr != serviceMap.end()) {
        serviceMap.erase(itr);
    }
}

ProtobufServicePtr& ProtobufServiceRegister::getService(const std::string& name) {
    if (name.empty()) { return nullService; }

    return serviceMap[name];
}

const ProtobufServiceRegister::MethodDescriptor*
ProtobufServiceRegister::getMethodDescriptor(const ProtobufServicePtr& service,
        const std::string& method) {
    if (!service) { return NULL; }

    const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();

    if (desc) {
        return desc->FindMethodByName(method);
    }

    return NULL;
}

int protobuf::ProtobufServiceRegister::getRegisteredServices(std::vector<std::string>* names) {
    if (NULL == names) { return 0; }

    int count = 0;
    ServiceMap::const_iterator itr = serviceMap.begin();

    for (; itr != serviceMap.end(); ++itr) {
        names->push_back(itr->first);
        ++count;
    }

    return count;
}

}
}
}
}