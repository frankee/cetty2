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

#include <vector>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/stubs/common.h>

#include <cetty/Types.h>
#include <cetty/protobuf/service/ProtobufFieldTraits.h>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace protobuf {
namespace service {

using namespace google::protobuf;

class ProtobufUtil {
public:
    typedef boost::variant<int64_t,
            double,
            const std::string*,
            const Message*,
            std::vector<int64_t>,
            std::vector<double>,
            std::vector<const std::string*>,
            std::vector<const Message*> > FieldValue;

    typedef boost::function2<uint32_t, const uint8_t*, int> ChecksumFunction;

public:
    static ChecksumFunction adler32Checksum;

    template<typename OptionType, typename DescriptorType>
    static boost::optional<OptionType> getOption(const DescriptorType* descriptor,
            const std::string& option) {
        if (descriptor) {
            const Message& options = descriptor->options();
            return getOptionField<OptionType>(options, option);
        }
        else {
            return boost::optional<OptionType>();
        }
    }

    template<typename OptionType, typename DescriptorType>
    static boost::optional<OptionType> getOption(const DescriptorType* descriptor,
            const std::string& option,
            const std::string& field) {
        boost::optional<Message const*> msg =
            getOption<Message const*>(descriptor, option);

        if (msg && boost::get<Message const*>(msg)) {
            return getOptionField<OptionType>(**msg, field);
        }

        return boost::optional<OptionType>();
    }

    template<typename T>
    static boost::optional<T> getOptionField(const Message& message,
            const std::string& field) {
        const Reflection* reflection = message.GetReflection();
        const Descriptor* descriptor = message.GetDescriptor();

        std::vector<const FieldDescriptor*> fields;
        reflection->ListFields(message, &fields);

        for (std::size_t i = 0; i < fields.size(); ++i) {
            const FieldDescriptor* f = fields[i];

            if (f->name().compare(field) != 0) {
                continue;
            }

            if (!f->is_repeated() &&
                    f->cpp_type() == ProtobufFieldTraits<T>::FIELD_TYPE) {
                return ProtobufFieldTraits<T>::getField(message, reflection, f);
            }
        }

        return boost::optional<T>();
    }

    static FieldValue getMessageFieldValue(const Message& message,
                                           const std::string& name);

    static void logHandler(google::protobuf::LogLevel level,
                           const char* filename,
                           int line,
                           const std::string& message);

private:
    ProtobufUtil();
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFUTIL_H)

// Local Variables:
// mode: c++
// End:
