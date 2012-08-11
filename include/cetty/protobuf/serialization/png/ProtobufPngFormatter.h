#if !defined(CETTY_PROTOBUF_SERIALIZATION_PNG_PROTOBUFPNGFORMATTER_H)
#define CETTY_PROTOBUF_SERIALIZATION_PNG_PROTOBUFPNGFORMATTER_H

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

#include <cetty/protobuf/serialization/ProtobufFormatter.h>

namespace cetty {
namespace protobuf {
namespace serialization {
namespace png {

class ProtobufPngFormatter : public ProtobufFormatter {
public:
    ProtobufPngFormatter() {}
    virtual ~ProtobufPngFormatter() {}

    // not supported.
    virtual void format(boost::int64_t value, std::string* str);
    virtual void format(boost::int64_t value, const ChannelBufferPtr& buffer);

    // only support this format type
    virtual void format(const std::string& value, std::string* str);
    virtual void format(const std::string& value, const ChannelBufferPtr& buffer);

    // not supported.
    virtual void format(const google::protobuf::Message& value, std::string* str);
    virtual void format(const google::protobuf::Message& value, const ChannelBufferPtr& buffer);

    // not supported.
    virtual void format(std::vector<boost::int64_t>& value, std::string* str);
    virtual void format(std::vector<boost::int64_t>& value, const ChannelBufferPtr& buffer);

    // not supported.
    virtual void format(std::vector<const std::string*>& value, std::string* str);
    virtual void format(std::vector<const std::string*>& value, const ChannelBufferPtr& buffer);

    // not supported.
    virtual void format(std::vector<const google::protobuf::Message*>& value, std::string* str);
    virtual void format(std::vector<const google::protobuf::Message*>& value, const ChannelBufferPtr& buffer);

private:
    bool isPng(const std::string& value);

};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERIALIZATION_PNG_PROTOBUFPNGFORMATTER_H)

// Local Variables:
// mode: c++
// End:
