#include <cetty/handler/rpc/protobuf/ProtobufBsonParser.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include <mongodb/db/json.h>

namespace cetty { namespace handler { namespace rpc { namespace protobuf { 

	using namespace google::protobuf;
	using namespace mongo;

	void ProtobufBsonParser::parseFromBsonObj( const BSONObj& obj, google::protobuf::Message* message )
	{
		if(obj.isEmpty() || !message) {
			return;
		}

		const google::protobuf::Reflection* reflection = message->GetReflection();
		const google::protobuf::Descriptor* descriptor = message->GetDescriptor();

		int fieldCnt = descriptor->field_count();
		for (int i = 0; i < fieldCnt; ++i) {
			const google::protobuf::FieldDescriptor* field = descriptor->field(i);
			const std::string& fieldName = field->camelcase_name();
			if (!fieldName.empty() && obj.hasField(fieldName.c_str())) {
				BSONElement element = obj.getField(fieldName.c_str());
				if (!parseField(message, field, element, fieldName)) {
					return;
				}
			}
		}
	}

	bool ProtobufBsonParser::parseField( google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field, const BSONElement& element, const std::string& fieldName )
	{
		const google::protobuf::Reflection* reflection = message->GetReflection();

		if (field->is_extension()) {
			//TODO
		}
		else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
			// Groups must be serialized with their original capitalization.
		}

		if (field->is_repeated()) {
			if(mongo::Array != element.type()) {
				printf("protobuf field is repeated, but BsonElement not.\n");
				return false;
			}
			vector<BSONElement> elements = element.Array();
			size_t count = elements.size();

			FieldDescriptor::CppType cppType = field->cpp_type();
			switch(cppType) {
			case FieldDescriptor::CPPTYPE_UINT32:
			case FieldDescriptor::CPPTYPE_INT32: {

				for(size_t i = 0; i < count; ++i) {
					reflection->AddInt32(message, field, elements[i].Int());
				}
				break;
												 }		
			case FieldDescriptor::CPPTYPE_UINT64:
			case FieldDescriptor::CPPTYPE_INT64: {

				for(size_t i = 0; i < count; ++i) {
					reflection->AddInt64(message, field, elements[i].Long());
				}
				break;
												 }
			case FieldDescriptor::CPPTYPE_DOUBLE: {

				for(size_t i = 0; i < count; ++i) {
					reflection->AddDouble(message, field, elements[i].Double());
				}
				break;
												 }
			case FieldDescriptor::CPPTYPE_STRING: {

				for(size_t i = 0; i < count; ++i) {
					std::string str = element.toString(false,true);
					str = str.substr(1,str.length() - 2);
					reflection->AddString(message, field, str/*elements[i].String()*/);
				}
				break;
												 }
			case FieldDescriptor::CPPTYPE_MESSAGE: {

				for(size_t i = 0; i < count; ++i) {
					google::protobuf::Message* msg = reflection->AddMessage(message, field);
					parseFromBsonObj(elements[i].Obj(),msg);
				}
				break;
												 }
			case FieldDescriptor::CPPTYPE_BOOL: {

				for(size_t i = 0; i < count; ++i) {
					reflection->AddBool(message, field, elements[i].Bool());
				}
				break;
												 }
			default: {
				break;
					 }

			}
		}
		else {
			switch (field->cpp_type()) {
			case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
				reflection->SetInt32(message, field, element.Int());
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
				reflection->SetInt64(message, field, element.Long());
				break;
			case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
				reflection->SetDouble(message, field, element.Double());
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
				{
					std::string str = element.toString(false,true);
					str = str.substr(1,str.length() - 2);
					reflection->SetString(message, field, str);
				}
				break;
			case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
				google::protobuf::Message* msg = reflection->MutableMessage(message, field);
				parseFromBsonObj(element.Obj(),msg);
				break;
			}
		}
		return true;

	}

	void ProtobufBsonParser::parseFromBsonObjWithJson( const BSONObj& obj, google::protobuf::Message* message ) {
		ProtobufJsonParser parser;
		std::string str = obj.jsonString();
		parser.parseFromString(str,message);
	}

}}}}

