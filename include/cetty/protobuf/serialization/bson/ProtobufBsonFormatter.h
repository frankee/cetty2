#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFBSONFORMATTER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFBSONFORMATTER_H

#include <string>

#include "ProtobufJsonFormatter.h"

namespace mongo {
	class BSONObj;
	class BSONObjBuilder;
}
namespace google { namespace protobuf {
	class Message;
	class FieldDescriptor;
}}

namespace cetty { namespace handler { namespace rpc { namespace protobuf {

	using namespace mongo;

	class ProtobufBsonFormatter {
	public:
		static void serializeToBsonObjWithJsonString(const google::protobuf::Message& message, BSONObj* obj);

		static void serializeToBsonObj(const google::protobuf::Message& message, BSONObj* obj);

	private:
		static void serializeMessage(const google::protobuf::Message& message, BSONObjBuilder& builder);
		static bool serializeField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, BSONObjBuilder& builder);
		static void serializeSingleField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, BSONObjBuilder& builder);
	};

}}}}


#endif //CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFBSONFORMATTER_H