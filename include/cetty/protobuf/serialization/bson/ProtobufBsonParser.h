#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFBSONPARSER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFBSONPARSER_H

#include <string>

#include "ProtobufJsonParser.h"

namespace mongo {
	class BSONObj;
	class BSONElement;
}

namespace google { namespace protobuf {
	class FieldDescriptor;
	class Message;
}}

namespace cetty { namespace handler { namespace rpc { namespace protobuf { 

	 using namespace mongo;

	class ProtobufBsonParser {
	public:
		static void parseFromBsonObjWithJson(const BSONObj& obj, google::protobuf::Message* message);

		static void parseFromBsonObj(const BSONObj& obj, google::protobuf::Message* message);

	private:
		static bool parseField(google::protobuf::Message* message,
			const google::protobuf::FieldDescriptor* field,
			const BSONElement& element,
			const std::string& fieldName);
 	};

}}}}





#endif //CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFBSONPARSER_H
