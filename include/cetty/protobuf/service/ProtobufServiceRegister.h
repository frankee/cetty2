#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEREGISTER_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEREGISTER_H

#include <map>
#include <vector>
#include <string>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>

namespace google {
namespace protobuf {
class Message;
class Service;
class MethodDescriptor;
}
}

namespace cetty {
namespace protobuf {
namespace service {

using google::protobuf::Message;
using google::protobuf::Service;
using google::protobuf::MethodDescriptor;

class ProtobufServiceRegister {
public:
    typedef std::map<std::string, ProtobufServicePtr> ServiceMap;
    typedef ServiceMap::iterator ServiceIterator;
    typedef ServiceMap::const_iterator ServiceConstIterator;

public:
    static ProtobufServiceRegister& instance();

public:
    int  registerService(const ProtobufServicePtr& service);
    void unregisterService(const std::string& name);

    const ProtobufServicePtr& getService(const std::string& name) const;
    int getRegisteredServices(std::vector<std::string>* names) const;

    ServiceIterator serviceBegin() {
        return serviceMap.begin();
    }
    ServiceConstIterator serviceBegin() const {
        return serviceMap.begin();
    }
    ServiceIterator serviceEnd() {
        return serviceMap.end();
    }
    ServiceConstIterator serviceEnd() const {
        return serviceMap.end();
    }

    const MethodDescriptor* getMethodDescriptor(const ProtobufServicePtr& service,
            const std::string& method) const;

    const Message* getRequestPrototype(const ProtobufServicePtr& service,
                                       const std::string& method) const;

    const Message* getResponsePrototype(const ProtobufServicePtr& service,
                                        const std::string& method) const;

    const Message* getRequestPrototype(const std::string& service,
                                       const std::string& method) const {
        ServiceMap::const_iterator itr = serviceMap.find(service);

        if (itr != serviceMap.end()) {
            return getRequestPrototype(itr->second, method);
        }

        return NULL;
    }
    const Message* getResponsePrototype(const std::string& service,
                                        const std::string& method) const {
        ServiceMap::const_iterator itr = serviceMap.find(service);

        if (itr != serviceMap.end()) {
            return getResponsePrototype(itr->second, method);
        }

        return NULL;
    }

private:
    static ProtobufServicePtr nullService;

private:
    ProtobufServiceRegister();
    ~ProtobufServiceRegister();

    ProtobufServiceRegister(const ProtobufServiceRegister&);
    ProtobufServiceRegister& operator=(const ProtobufServiceRegister&);

private:
    ServiceMap serviceMap;
};

}
}
}


#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEREGISTER_H)

// Local Variables:
// mode: c++
// End:
