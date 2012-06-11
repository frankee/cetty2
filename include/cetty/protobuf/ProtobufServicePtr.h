#if !defined(CETTY_PROTOBUF_PROTOBUFSERVICEPTR_H)
#define CETTY_PROTOBUF_PROTOBUFSERVICEPTR_H

#include <boost/shared_ptr.hpp>

namespace google { namespace protobuf {
    class Service;
}}

namespace cetty { namespace protobuf { 

typedef boost::shared_ptr<google::protobuf::Service> ProtobufServicePtr;

}}

#endif //#if !defined(CETTY_PROTOBUF_PROTOBUFSERVICEPTR_H)
