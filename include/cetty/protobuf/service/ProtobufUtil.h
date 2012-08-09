#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFUTIL_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFUTIL_H

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

namespace cetty {
namespace protobuf {
namespace service {

class ProtobufUtil {
public:
    static google::protobuf::Message* getSubMessage(const std::string& name, google::protobuf::Message* message) {
        if (!message) {
            return NULL;
        }

        std::string fieldName;
        std::string subFieldName;
        std::string::size_type pos = name.find(".");

        if (pos != name.npos) {
            fieldName = name.substr(0, pos);
            subFieldName = name.substr(pos);
        }
        else {
            fieldName = name;
        }

        const google::protobuf::Reflection* reflection = message->GetReflection();
        const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
        int fieldCnt = descriptor->field_count();

        for (int i = 0; i < fieldCnt; ++i) {
            const google::protobuf::FieldDescriptor* field = descriptor->field(i);

            if (field->name() == fieldName) {
                if (subFieldName.empty()) {
                    return reflection->MutableMessage(message, field);
                }
                else {
                    return getSubMessage(subFieldName, reflection->MutableMessage(message, field));
                }
            }
        }

        return NULL;
    }
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFUTIL_H)

// Local Variables:
// mode: c++
// End:
