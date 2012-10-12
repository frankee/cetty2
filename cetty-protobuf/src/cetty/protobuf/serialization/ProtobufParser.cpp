
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

#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/protobuf/serialization/json/ProtobufJsonParser.h>

namespace cetty {
namespace protobuf {
namespace serialization {

using namespace cetty::buffer;

std::map<std::string, ProtobufParser*> ProtobufParser::parsers;

struct ProtobufParserRegister {
    ProtobufParserRegister() {
#if JSON_PARSER_PLUGIN
        ProtobufParser::registerParser("json", new json::ProtobufJsonParser);
#endif
    }
    ~ProtobufParserRegister() {
#if JSON_PARSER_PLUGIN
       unregister("json");
#endif
    }

    void unregister(const std::string& name) {
        ProtobufParser* parser = ProtobufParser::getParser(name);
        ProtobufParser::unregisterParser(name);
        if (parser) {
            delete parser;
        }
    }

} parserRegister;

int ProtobufParser::parse(const std::string& buffer, Message* message) {
    return parse(buffer.c_str(), (int)buffer.size(), message);
}

int ProtobufParser::parse(const ChannelBufferPtr& buffer, Message* message) {
    StringPiece bytes;
    buffer->readableBytes(&bytes);
    return parse(bytes.data(), bytes.size(), message);
}

ProtobufParser* ProtobufParser::getParser(const std::string& name) {
    std::map<std::string, ProtobufParser*>::const_iterator i
        = parsers.find(name);

    if (i != parsers.end()) {
        return i->second;
    }

    return NULL;
}

void ProtobufParser::registerParser(const std::string& name, ProtobufParser* parser) {
    if (!name.empty() && parser) {
        parsers.insert(std::make_pair(name, parser));
    }
}

void ProtobufParser::unregisterParser(const std::string& name) {
    std::map<std::string, ProtobufParser*>::iterator itr = parsers.find(name);
    if (itr != parsers.end()) {
        parsers.erase(itr);
    }
}

}
}
}
