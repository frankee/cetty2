#if !defined(CETTY_PROTOBUF_SERIALIZATION_PROTOBUFFORMATTER_H)
#define CETTY_PROTOBUF_SERIALIZATION_PROTOBUFFORMATTER_H

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

#include <map>
#include <vector>
#include <string>
#include <boost/cstdint.hpp>
#include <cetty/buffer/ChannelBufferPtr.h>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace protobuf {
namespace serialization {

using namespace cetty::buffer;

class ProtobufFormatter {
public:
    virtual ~ProtobufFormatter() {}

    void format(const std::string& key,
                boost::int64_t value,
                std::string* str);
    void format(const std::string& key,
                boost::int64_t value,
                const ChannelBufferPtr& buffer);

    void format(const std::string& key,
                double value,
                std::string* str);
    void format(const std::string& key,
                double value,
                const ChannelBufferPtr& buffer);

    // if the value is binary, in json or xml will using base64
    virtual void format(const std::string& key,
                        const std::string& value,
                        std::string* str) = 0;
    virtual void format(const std::string& key,
                        const std::string& value,
                        const ChannelBufferPtr& buffer) = 0;

    // if there is a formated field in message (using the field option),
    // the the formatter will using the formated str.
    virtual void format(const google::protobuf::Message& value,
                        std::string* str) = 0;
    virtual void format(const google::protobuf::Message& value,
                        const ChannelBufferPtr& buffer) = 0;

    void format(const std::string& key,
                const std::vector<boost::int64_t>& value,
                std::string* str);
    void format(const std::string& key,
                const std::vector<boost::int64_t>& value,
                const ChannelBufferPtr& buffer);

    void format(const std::string& key,
                const std::vector<double>& value,
                std::string* str);
    void format(const std::string& key,
                const std::vector<double>& value,
                const ChannelBufferPtr& buffer);

    virtual void format(const std::string& key,
                        const std::vector<const std::string*>& value,
                        std::string* str) = 0;
    virtual void format(const std::string& key,
                        const std::vector<const std::string*>& value,
                        const ChannelBufferPtr& buffer) = 0;

    virtual void format(const std::string& key,
                        const std::vector<const google::protobuf::Message*>& value,
                        std::string* str) = 0;
    virtual void format(const std::string& key,
                        const std::vector<const google::protobuf::Message*>& value,
                        const ChannelBufferPtr& buffer) = 0;

public:
    static ProtobufFormatter* getFormatter(const std::string& name);

    static void registerFormatter(const std::string& name,
                                  ProtobufFormatter* formatter);
    static void unregisterFormatter(const std::string& name);

private:
    static std::map<std::string, ProtobufFormatter*> formatters;
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERIALIZATION_PROTOBUFFORMATTER_H)

// Local Variables:
// mode: c++
// End:
