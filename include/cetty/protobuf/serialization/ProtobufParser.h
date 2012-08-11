#if !defined(CETTY_PROTOBUF_SERIALIZATION_PROTOBUFPARSER_H)
#define CETTY_PROTOBUF_SERIALIZATION_PROTOBUFPARSER_H

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
#include <string>
#include <cetty/buffer/ChannelBufferPtr.h>

namespace google {
namespace protobuf {
class Message;
}
}

namespace cetty {
namespace protobuf {
namespace serialization {

using ::google::protobuf::Message;
using namespace cetty::buffer;

class ProtobufParser {
public:
    virtual ~ProtobufParser() {}

    virtual int parse(const char* buffer, Message* message) = 0;

    int parse(const std::string& buffer, Message* message);
    int parse(const ChannelBufferPtr& buffer, Message* message);

public:
    static ProtobufParser* getParser(const std::string& name);

    static void registerParser(const std::string& name, ProtobufParser* parser);
    static void unregisterParser(const std::string& name);

private:
    static std::map<std::string, ProtobufParser*> parsers;
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERIALIZATION_PROTOBUFPARSER_H)

// Local Variables:
// mode: c++
// End:
