#include <cetty/handler/rpc/protobuf/ProtobufBsonFormatter.h>

#include <stdio.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include <mongodb/db/json.h>

#if defined GetMessage
#undef GetMessage
#endif

namespace cetty { namespace handler { namespace rpc { namespace protobuf { 
	using namespace std;
	using namespace mongo;

	using namespace google::protobuf;
	using namespace google::protobuf;

void ProtobufBsonFormatter::serializeToBsonObj( const google::protobuf::Message& message, BSONObj* obj )
{
	BSONObjBuilder objBuilder;
	serializeMessage(message,objBuilder);
	*obj = objBuilder.obj();
}

void ProtobufBsonFormatter::serializeMessage( const google::protobuf::Message& message, BSONObjBuilder& builder )
{
	const google::protobuf::Reflection* reflection = message.GetReflection();
	const google::protobuf::Descriptor* descriptor = message.GetDescriptor();

	int fieldCnt = descriptor->field_count();
	for (int i = 0; i < fieldCnt; ++i) {
		bool serialized = serializeField(message, descriptor->field(i), builder);
	}
}

bool ProtobufBsonFormatter::serializeField( const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, BSONObjBuilder& builder )
{
	const google::protobuf::Reflection* reflection = message.GetReflection();
	//FIXME
	if (field->is_repeated() || reflection->HasField(message, field)) {
		serializeSingleField(message, field, builder);
		return true;
	}
	return false;
}


void ProtobufBsonFormatter::serializeSingleField( const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, BSONObjBuilder& builder )
{
	std::string fieldName("");
	if (field->is_extension()) {
		//TODO
	}
	else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
		// Groups must be serialized with their original capitalization.
		fieldName = field->message_type()->name().c_str();
		//...append values
	}
	else {
		fieldName = field->camelcase_name();
		const google::protobuf::Reflection* reflection = message.GetReflection();

		if(field->is_repeated()) {
			int fieldsize = reflection->FieldSize(message, field);
			switch(field->cpp_type()) {
			case FieldDescriptor::CPPTYPE_INT32:	{		//= 1,     // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
				std::vector<int32> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedInt32(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_INT64:	{		//= 2,     // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
				std::vector<int64> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedInt64(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_UINT32:	{		//= 3,     // TYPE_UINT32, TYPE_FIXED32
				std::vector<uint32> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedUInt32(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_UINT64:	{		//= 4,     // TYPE_UINT64, TYPE_FIXED64
				std::vector<long long> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back((long long)reflection->GetRepeatedUInt64(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_DOUBLE:	{		//= 5,     // TYPE_DOUBLE
				std::vector<double> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedDouble(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_FLOAT:	{		//= 6,     // TYPE_FLOAT
				std::vector<float> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedFloat(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_BOOL:		{		//= 7,     // TYPE_BOOL
				std::vector<bool> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedBool(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_STRING:	{		//= 9,     // TYPE_STRING, TYPE_BYTES

				std::vector<std::string> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedString(message,field,i));
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_ENUM:		{       //= 8,     // TYPE_ENUM
				std::vector<std::string> values;
				values.reserve(fieldsize);
				for(int i = 0; i < fieldsize; ++i) {
					values.push_back(reflection->GetRepeatedEnum(message,field,i)->name());
				}
				builder.append(fieldName,values);

				break;
													}
			case FieldDescriptor::CPPTYPE_MESSAGE:	{		//= 10,    // TYPE_MESSAGE, TYPE_GROUP
				BSONObjBuilder sub (builder.subarrayStart(fieldName));
                for(int i = 0; i < fieldsize; ++i)  {
					char number[16] = {0};
					sprintf(number, "%d", i);
                    BSONObjBuilder obj = sub.subobjStart(number);
					serializeMessage(reflection->GetRepeatedMessage(message, field, i), obj);
                    obj.done();
				}
                sub.done();

				break;
													}
			default:								{
				break;
													}
			}// end switch
		}
		else { //not repeated
			switch(/*cppType*/field->cpp_type()) {
			case FieldDescriptor::CPPTYPE_INT32:	{		//= 1,     // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
				builder.append(fieldName,reflection->GetInt32(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_INT64:	{		//= 2,     // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
				builder.append(fieldName,reflection->GetInt64(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_UINT32:	{		//= 3,     // TYPE_UINT32, TYPE_FIXED32
				builder.append(fieldName,reflection->GetUInt32(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_UINT64:	{		//= 4,     // TYPE_UINT64, TYPE_FIXED64
				builder.append(fieldName,(long long)reflection->GetUInt64(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_DOUBLE:	{		//= 5,     // TYPE_DOUBLE
				builder.append(fieldName,reflection->GetDouble(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_FLOAT:	{		//= 6,     // TYPE_FLOAT
				builder.append(fieldName,reflection->GetFloat(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_BOOL:		{		//= 7,     // TYPE_BOOL
				builder.append(fieldName,reflection->GetBool(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_STRING:	{		//= 9,     // TYPE_STRING, TYPE_BYTES
				builder.append(fieldName,reflection->GetString(message,field));
				break;
													}
			case FieldDescriptor::CPPTYPE_ENUM:		{       //= 8,     // TYPE_ENUM
				builder.append(fieldName,reflection->GetEnum(message,field)->name());
				break;
													}
			case FieldDescriptor::CPPTYPE_MESSAGE:	{		//= 10,    // TYPE_MESSAGE, TYPE_GROUP
                BSONObjBuilder sub (builder.subobjStart(fieldName));
                serializeMessage(reflection->GetMessage(message, field), sub);
                sub.done();
				break;
													}
			default:								{
				break;
													}
			}// end switch
		}
	} //end else



}

void ProtobufBsonFormatter::serializeToBsonObjWithJsonString( const google::protobuf::Message& message, BSONObj* obj ) {
	ProtobufJsonFormatter formatter;
	std::string str("");
	formatter.serializeToString(message, &str);
	*obj = fromjson(str);
}


}}}}
