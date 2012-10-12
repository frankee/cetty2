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

#include <cetty/util/StringUtil.h>
#include <cetty/protobuf/serialization/json/ProtobufJsonFormatter.h>
#include <cetty/protobuf/serialization/png/ProtobufPngFormatter.h>

namespace cetty {
namespace protobuf {
namespace serialization {

using namespace cetty::util;

std::map<std::string, ProtobufFormatter*> ProtobufFormatter::formatters;

struct ProtobufFormatterRegister {
    ProtobufFormatterRegister() {
#if (JSON_FORMATTER_PLUGIN)
        ProtobufFormatter::registerFormatter("json", new json::ProtobufJsonFormatter);
#endif

#if (PNG_FORMATTER_PLUGIN)
        ProtobufFormatter::registerFormatter("png", new png::ProtobufPngFormatter);
#endif
    }
    ~ProtobufFormatterRegister() {
#if (JSON_FORMATTER_PLUGIN)
        unregister("json");
#endif

#if (PNG_FORMATTER_PLUGIN)
        unregister("png");
#endif
    }

    void unregister(const std::string& name) {
        ProtobufFormatter* formatter = ProtobufFormatter::getFormatter(name);
        ProtobufFormatter::unregisterFormatter(name);

        if (formatter) {
            delete formatter;
        }
    }

} formatterRegister;

void ProtobufFormatter::format(const std::string& key,
                               int64_t value,
                               std::string* str) {
    std::string v;
    StringUtil::strprintf(&v, "%lld", key);
    format(key, v, str);
}

void ProtobufFormatter::format(const std::string& key,
                               int64_t value,
                               const ChannelBufferPtr& buffer) {
    std::string v;
    StringUtil::strprintf(&v, "%lld", key);
    format(key, v, buffer);
}

void ProtobufFormatter::format(const std::string& key,
                               double value,
                               std::string* str) {
    std::string v;
    StringUtil::strprintf(&v, "%lf", key);
    format(key, v, str);
}

void ProtobufFormatter::format(const std::string& key,
                               double value,
                               const ChannelBufferPtr& buffer) {
    std::string v;
    StringUtil::strprintf(&v, "%lf", key);
    format(key, v, buffer);
}

void ProtobufFormatter::format(const std::string& key,
                               const std::vector<int64_t>& value,
                               std::string* str) {
    std::vector<std::string> strValues;
    std::size_t j = value.size();
    strValues.resize(j);

    for (std::size_t i = 0; i < j; ++i) {
        StringUtil::strprintf(&strValues[i], "%lld", value[i]);
    }

    format(key, strValues, str);
}

void ProtobufFormatter::format(const std::string& key,
                               const std::vector<int64_t>& value,
                               const ChannelBufferPtr& buffer) {
    std::vector<std::string> strValues;
    std::size_t j = value.size();
    strValues.resize(j);

    for (std::size_t i = 0; i < j; ++i) {
        StringUtil::strprintf(&strValues[i], "%lld", value[i]);
    }

    format(key, strValues, buffer);
}

void ProtobufFormatter::format(const std::string& key,
                               const std::vector<double>& value,
                               std::string* str) {
    std::vector<std::string> strValues;
    std::size_t j = value.size();
    strValues.resize(j);

    for (std::size_t i = 0; i < j; ++i) {
        StringUtil::strprintf(&strValues[i], "%lf", value[i]);
    }

    format(key, strValues, str);
}

void ProtobufFormatter::format(const std::string& key,
                               const std::vector<double>& value,
                               const ChannelBufferPtr& buffer) {
    std::vector<std::string> strValues;
    std::size_t j = value.size();
    strValues.resize(j);

    for (std::size_t i = 0; i < j; ++i) {
        StringUtil::strprintf(&strValues[i], "%lf", value[i]);
    }

    format(key, strValues, buffer);
}

void ProtobufFormatter::format(const std::string& key,
                               const std::vector<std::string>& value,
                               std::string* str) {
    std::vector<const std::string*> ptrValues;
    std::size_t j = value.size();
    ptrValues.reserve(j);

    for (std::size_t i = 0; i < j; i < j) {
        ptrValues.push_back(&value[i]);
    }

    format(key, ptrValues, str);
}

void ProtobufFormatter::format(const std::string& key,
                               const std::vector<std::string>& value,
                               const ChannelBufferPtr& buffer) {
    std::vector<const std::string*> ptrValues;
    std::size_t j = value.size();
    ptrValues.reserve(j);

    for (std::size_t i = 0; i < j; i < j) {
        ptrValues.push_back(&value[i]);
    }

    format(key, ptrValues, buffer);
}

ProtobufFormatter* ProtobufFormatter::getFormatter(const std::string& name) {
    std::map<std::string, ProtobufFormatter*>::const_iterator itr
        = formatters.find(name);

    if (itr != formatters.end()) {
        return itr->second;
    }

    return NULL;
}

void ProtobufFormatter::registerFormatter(const std::string& name,
        ProtobufFormatter* formatter) {
    if (!name.empty() && NULL != formatter) {
        formatters.insert(std::make_pair(name, formatter));
    }
}

void ProtobufFormatter::unregisterFormatter(const std::string& name) {
    std::map<std::string, ProtobufFormatter*>::iterator itr
        = formatters.find(name);

    if (itr != formatters.end()) {
        formatters.erase(itr);
    }
}

}
}
}
