#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEREGISTER_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEREGISTER_H

#include <map>
#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
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

class ProtobufServiceRegister : private boost::noncopyable {
public:
    typedef std::map<std::string, ProtobufServicePtr> ServiceMap;
    typedef std::map<std::string, const Message*> ResponsePrototypeMap;
    typedef ServiceMap::iterator ServiceIterator;
    typedef ServiceMap::const_iterator ServiceConstIterator;

    typedef boost::function<void (ProtobufServicePtr const&)> ServiceRegisteredCallback;
    typedef std::vector<ServiceRegisteredCallback> ServiceRegisteredCallbacks;

public:
    static ProtobufServiceRegister& instance();

public:
    int  registerService(const ProtobufServicePtr& service);
    void unregisterService(const std::string& name);

    int registerResponsePrototype(const std::string& service,
                                  const std::string& method,
                                  const Message* proto);

    void unregisterResponsePrototype(const std::string& service,
                                     const std::string& method);

    void registerServiceRegisteredCallback(
        const ServiceRegisteredCallback& callback);

    int getServices(std::vector<std::string>* names) const;
    const ProtobufServicePtr& getService(const std::string& name) const;

    ServiceIterator serviceBegin();
    ServiceIterator serviceEnd();

    ServiceConstIterator serviceBegin() const;
    ServiceConstIterator serviceEnd() const;

    const MethodDescriptor* getMethodDescriptor(
        const ProtobufServicePtr& service,
        const std::string& method) const;

    const Message* getRequestPrototype(const ProtobufServicePtr& service,
                                       const std::string& method) const;

    const Message* getResponsePrototype(const ProtobufServicePtr& service,
                                        const std::string& method) const;

    const Message* getRequestPrototype(const std::string& service,
                                       const std::string& method) const;

    const Message* getResponsePrototype(const std::string& service,
                                        const std::string& method) const;

private:
    static ProtobufServicePtr nullService;

private:
    ProtobufServiceRegister();
    ~ProtobufServiceRegister();

private:
    ServiceMap serviceMap_;
    ResponsePrototypeMap responsePrototypeMap_;

    ServiceRegisteredCallbacks callbacks_;
};

inline
ProtobufServiceRegister::ServiceIterator ProtobufServiceRegister::serviceBegin() {
    return serviceMap_.begin();
}

inline
ProtobufServiceRegister::ServiceConstIterator ProtobufServiceRegister::serviceBegin() const {
    return serviceMap_.begin();
}

inline
ProtobufServiceRegister::ServiceIterator ProtobufServiceRegister::serviceEnd() {
    return serviceMap_.end();
}

inline
ProtobufServiceRegister::ServiceConstIterator ProtobufServiceRegister::serviceEnd() const {
    return serviceMap_.end();
}

}
}
}


#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFSERVICEREGISTER_H)

// Local Variables:
// mode: c++
// End:
