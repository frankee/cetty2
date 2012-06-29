#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFPARSER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFPARSER_H

#include <string>

namespace google { namespace protobuf {
    class Message;
}}

namespace cetty { namespace protobuf { namespace serialization { 

	using ::google::protobuf::Message;
	
	template<typename T>
    class ProtobufParser {
    public:
        virtual ~ProtobufParser() {}

		virtual int parse(const void* buffer, int size, Message* message) = 0;
		virtual int parse(const ChannelBufferPtr& buffer, Message* message);
		
		
        virtual int parse(const T& obj, Message* message) = 0;
    };

}}}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFPARSER_H)
