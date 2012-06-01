#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFPARSER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFPARSER_H

#include <string>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

class ProtobufParser {
public:
    virtual ~ProtobufParser() {}

    virtual void parseFromString(const std::string& str, google::protobuf::Message* message) = 0;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFPARSER_H)

// Local Variables:
// mode: c++
// End:

