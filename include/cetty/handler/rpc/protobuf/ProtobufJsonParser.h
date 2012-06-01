#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFJSONPARSER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFJSONPARSER_H

#include <cetty/handler/rpc/protobuf/ProtobufParser.h>

namespace google {
namespace protobuf {
class FieldDescriptor;
}
}

namespace Json {
class Value;
}

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

class ProtobufJsonParser : public ProtobufParser {
public:
    ProtobufJsonParser() {}
    virtual ~ProtobufJsonParser() {}

    virtual void parseFromString(const std::string& str, google::protobuf::Message* message);

private:
    bool parseMessage(google::protobuf::Message* message,
                      const Json::Value& value);

    bool parseField(google::protobuf::Message* message,
                    const google::protobuf::FieldDescriptor* field,
                    const Json::Value& value);
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFJSONPARSER_H)

// Local Variables:
// mode: c++
// End:

