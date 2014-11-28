#if !defined(CETTY_PROTOBUF_SERIALIZATION_JSON_PROTOBUFJSONFORMATTER_H)
#define CETTY_PROTOBUF_SERIALIZATION_JSON_PROTOBUFJSONFORMATTER_H

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

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/repeated_field.h>

#include <cetty/protobuf/serialization/ProtobufFormatter.h>

#define JSON_FORMATTER_PLUGIN 1

namespace cetty {
namespace protobuf {
namespace serialization {
namespace json {

using namespace cetty::protobuf::serialization;

template<typename T, int Style>
class JsonPrinter;

class ProtobufJsonFormatter : public ProtobufFormatter {
public:
    ProtobufJsonFormatter() : style(false) {}
    ProtobufJsonFormatter(bool style) : style(style) {}

    virtual ~ProtobufJsonFormatter() {}

    // if the value is binary, in json or xml will using base64
    virtual void format(const std::string& key,
                        const std::string& value,
                        std::string* str);
    virtual void format(const std::string& key,
                        const std::string& value,
                        const ChannelBufferPtr& buffer);

    // if there is a formated field in message (using the field option),
    // the the formatter will using the formated str.
    virtual void format(const google::protobuf::Message& value,
                        std::string* str);
    virtual void format(const google::protobuf::Message& value,
                        const ChannelBufferPtr& buffer);

    virtual void format(const std::string& key,
                        const std::vector<const std::string*>& value,
                        std::string* str);
    virtual void format(const std::string& key,
                        const std::vector<const std::string*>& value,
                        const ChannelBufferPtr& buffer);

    virtual void format(const std::string& key,
                        const std::vector<const google::protobuf::Message*>& value,
                        std::string* str);
    virtual void format(const std::string& key,
                        const std::vector<const google::protobuf::Message*>& value,
                        const ChannelBufferPtr& buffer);

	void format(const std::vector<const google::protobuf::Message*>& value,
		std::string* str);

	template<typename T>
	void format(const ::google::protobuf::RepeatedPtrField<T>& messages, std::string* str) {
		std::vector<const ::google::protobuf::Message*> msgs;
		for (int i = 0; i < messages.size(); ++i) {
			msgs.push_back(&messages.Get(i));
		}
		format(msgs, str);
	}

private:
    bool style;
};

}

using json::ProtobufJsonFormatter;

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERIALIZATION_JSON_PROTOBUFJSONFORMATTER_H)

// Local Variables:
// mode: c++
// End:
