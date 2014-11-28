#if !defined(CETTY_PROTOBUF_SERIALIZATION_JSON_PROTOBUFJSONPARSER_H)
#define CETTY_PROTOBUF_SERIALIZATION_JSON_PROTOBUFJSONPARSER_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/protobuf/serialization/ProtobufParser.h>

#define JSON_PARSER_PLUGIN 1

namespace google {
namespace protobuf {
class FieldDescriptor;
}
}

namespace YAML {
class Node;
}

namespace cetty {
namespace protobuf {
namespace serialization {
namespace json {

using namespace cetty::protobuf::serialization;

class ProtobufJsonParser : public ProtobufParser {
public:
    ProtobufJsonParser() {}
    virtual ~ProtobufJsonParser() {}

    virtual int parse(const char* buffer, int bufferSize, google::protobuf::Message* message);
    int parseMessage(const YAML::Node& node, google::protobuf::Message* message);

	bool parse(const std::string& buffer,
		const std::string& fieldName,
		google::protobuf::Message* message) {
			return parse(buffer.c_str(),
				static_cast<int>(buffer.size()),
				fieldName,
				message);
	}

	bool parse(const char* buffer,
		int bufferSize,
		const std::string& fieldName,
		google::protobuf::Message* message);
	
	bool parse(const YAML::Node& node,
		const std::string& fieldName,
		google::protobuf::Message* message);

private:
    int parseField(const YAML::Node& node,
                    const google::protobuf::FieldDescriptor* field,
                    google::protobuf::Message* message);
};


}

using json::ProtobufJsonParser;

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERIALIZATION_JSON_PROTOBUFJSONPARSER_H)

// Local Variables:
// mode: c++
// End:
