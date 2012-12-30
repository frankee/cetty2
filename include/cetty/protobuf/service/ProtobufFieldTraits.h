#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFFIELDTRAITS_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFFIELDTRAITS_H

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

#include <boost/optional.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <cetty/Types.h>

namespace cetty {
namespace protobuf {
namespace service {

using namespace google::protobuf;

template<typename T>
struct ProtobufFieldTraits {
    enum { FIELD_TYPE = FieldDescriptor::MAX_CPPTYPE + 1 };

    static boost::optional<T> getField(const Message& message,
                                       const Reflection* reflection,
                                       const FieldDescriptor* field) {
        return boost::optional<T>();
    }
};

template<>
struct ProtobufFieldTraits<int> {
    enum { FIELD_TYPE = FieldDescriptor::CPPTYPE_INT32 };

    static boost::optional<int> getField(const Message& message,
                                         const Reflection* reflection,
                                         const FieldDescriptor* field) {
        if (reflection->HasField(message, field)) {
            return reflection->GetInt32(message, field);
        }
        else {
            return boost::optional<int>();
        }
    }
};

template<>
struct ProtobufFieldTraits<int64_t> {
    enum { FIELD_TYPE = FieldDescriptor::CPPTYPE_INT64 };

    static boost::optional<int64_t> getField(const Message& message,
            const Reflection* reflection,
            const FieldDescriptor* field) {
        if (reflection->HasField(message, field)) {
            return reflection->GetInt64(message, field);
        }
        else {
            return boost::optional<int64_t>();
        }
    }
};

template<>
struct ProtobufFieldTraits<double> {
    enum { FIELD_TYPE = FieldDescriptor::CPPTYPE_DOUBLE };

    static boost::optional<double> getField(const Message& message,
                                            const Reflection* reflection,
                                            const FieldDescriptor* field) {
        if (reflection->HasField(message, field)) {
            return reflection->GetDouble(message, field);
        }
        else {
            return boost::optional<double>();
        }
    }
};

template<>
struct ProtobufFieldTraits<bool> {
    enum { FIELD_TYPE = FieldDescriptor::CPPTYPE_BOOL };

    static boost::optional<bool> getField(const Message& message,
                                          const Reflection* reflection,
                                          const FieldDescriptor* field) {
        if (reflection->HasField(message, field)) {
            return reflection->GetBool(message, field);
        }
        else {
            return boost::optional<bool>();
        }
    }
};

template<>
struct ProtobufFieldTraits<std::string> {
    enum { FIELD_TYPE = FieldDescriptor::CPPTYPE_STRING };

    static boost::optional<std::string> getField(const Message& message,
            const Reflection* reflection,
            const FieldDescriptor* field) {
        if (reflection->HasField(message, field)) {
            return reflection->GetString(message, field);
        }
        else {
            return boost::optional<std::string>();
        }
    }
};

template<>
struct ProtobufFieldTraits<std::string const*> {
    enum { FIELD_TYPE = FieldDescriptor::CPPTYPE_STRING };

    static boost::optional<std::string const*> getField(const Message& message,
            const Reflection* reflection,
            const FieldDescriptor* field) {
        if (reflection->HasField(message, field)) {
            return &reflection->GetStringReference(message, field, NULL);
        }
        else {
            return boost::optional<std::string const*>();
        }
    }
};

template<>
struct ProtobufFieldTraits<Message const*> {
    enum { FIELD_TYPE = FieldDescriptor::CPPTYPE_MESSAGE };

    static boost::optional<Message const*> getField(const Message& message,
            const Reflection* reflection,
            const FieldDescriptor* field) {
        if (reflection->HasField(message, field)) {
            return &reflection->GetMessage(message, field);
        }
        else {
            return boost::optional<Message const*>();
        }
    }
};

}
}
}


#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFFIELDTRAITS_H)

// Local Variables:
// mode: c++
// End:
