#include <cetty/protobuf/service/ProtobufServiceRegister.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>
#include <cetty/protobuf/service/ProtobufService.h>

namespace cetty {
namespace protobuf {
namespace service {

ProtobufServicePtr ProtobufServiceRegister::nullService;


ProtobufServiceRegister::ProtobufServiceRegister() {

}

ProtobufServiceRegister::~ProtobufServiceRegister() {

}

int ProtobufServiceRegister::registerService(const ProtobufServicePtr& service) {
    if (!service) {
        printf("the name or the service SHOULD NOT be NULL.\n");
        return -1;
    }

    std::string serviceName = service->GetDescriptor()->full_name();

    ServiceIterator itr = serviceMap.find(serviceName);

    if (itr != serviceMap.end()) {
        printf("the name of the service already registered, will be update.\n");
        serviceMap.erase(itr);
    }

    serviceMap[serviceName] = service;
    return 0;
}

void ProtobufServiceRegister::unregisterService(const std::string& name) {
    ServiceIterator itr = serviceMap.find(name);

    if (itr != serviceMap.end()) {
        serviceMap.erase(itr);
    }
}

const ProtobufServicePtr& ProtobufServiceRegister::getService(const std::string& name) const {
    if (name.empty()) {
        return nullService;
    }

    ServiceMap::const_iterator itr = serviceMap.find(name);

    if (itr != serviceMap.end()) {
        return itr->second;
    }

    return nullService;
}

const MethodDescriptor* ProtobufServiceRegister::getMethodDescriptor(
    const ProtobufServicePtr& service,
    const std::string& method) const {
    if (!service) { return NULL; }

    const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();

    if (desc) {
        return desc->FindMethodByName(method);
    }

    return NULL;
}

const Message* ProtobufServiceRegister::getRequestPrototype(
    const ProtobufServicePtr& service,
    const std::string& method) const {
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

const Message* ProtobufServiceRegister::getResponsePrototype(
    const ProtobufServicePtr& service,
    const std::string& method) const {
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

int ProtobufServiceRegister::getRegisteredServices(std::vector<std::string>* names) const {
    if (NULL == names) { return 0; }

    int count = 0;
    ServiceMap::const_iterator itr = serviceMap.begin();

    for (; itr != serviceMap.end(); ++itr) {
        names->push_back(itr->first);
        ++count;
    }

    return count;
}

ProtobufServiceRegister& ProtobufServiceRegister::instance() {
    static ProtobufServiceRegister serviceRegister;
    return serviceRegister;
}

}
}
}
