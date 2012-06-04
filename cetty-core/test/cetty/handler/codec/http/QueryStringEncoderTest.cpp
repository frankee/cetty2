/**
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include "gtest/gtest.h"
#include "cetty/handler/codec/http/QueryStringEncoder.h"

using namespace cetty::handler::codec::http;

TEST(QueryStringEncoderTest, testEncoder) {
    static const char* EncoderStr =
        "search?hl=zh-CN&newwindow=1&safe=strict&q=test+bss&aq=f&aqi=&aql=&oq=";

    QueryStringEncoder encoder("search");
    encoder.addParam("hl", "zh-CN");
    encoder.addParam("newwindow", "1");
    encoder.addParam("safe", "strict");
    encoder.addParam("q", "test+bss");
    encoder.addParam("aq", "f");
    encoder.addParam("aqi", "");
    encoder.addParam("aql", "");
    encoder.addParam("oq", "");
    
    ASSERT_STREQ(EncoderStr, encoder.toString().c_str());
}