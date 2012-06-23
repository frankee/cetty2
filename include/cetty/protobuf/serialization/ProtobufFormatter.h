#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFFORMATTER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFFORMATTER_H

#include <string>

namespace google { namespace protobuf {
    class Message;
}}

namespace cetty { namespace handler { namespace rpc { namespace protobuf { 

class ProtobufFormatter {
public:
	virtual ~ProtobufFormatter() {}

    virtual void serializeToString(const google::protobuf::Message& message, std::string* str) = 0;
};

}}}}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFFORMATTER_H)
