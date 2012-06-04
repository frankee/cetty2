/*
 * Copyright 2010 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

/**
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="http://tsunanet.net/">Benoit Sigoure</a>
 * @version $Rev: 2302 $, $Date: 2010-06-14 20:07:44 +0900 (Mon, 14 Jun 2010) $
 */

#include "gtest/gtest.h"
#include "cetty/handler/codec/http/QueryStringDecoder.h"

using namespace cetty::handler::codec::http;

TEST(QueryStringDecoderTest, testBasic) {
    QueryStringDecoder* d;
    const QueryStringDecoder::ParametersType* params;

    d = new QueryStringDecoder("/foo?a=b=c");
    ASSERT_STREQ("/foo", d->getPath().c_str());

    params = &(d->getParameters());
    ASSERT_EQ(1, params->size());
    ASSERT_EQ(1, params->find("a")->second.size());
    ASSERT_STREQ("b=c", params->find("a")->second.at(0).c_str());
    delete d;

    d = new QueryStringDecoder("/foo?a=1&a=2");
    ASSERT_STREQ("/foo", d->getPath().c_str());

    params = &(d->getParameters());
    ASSERT_EQ(1, params->size());
    ASSERT_EQ(2, params->find("a")->second.size());
    ASSERT_STREQ("1", params->find("a")->second.at(0).c_str());
    ASSERT_STREQ("2", params->find("a")->second.at(1).c_str());
    delete d;

    d = new QueryStringDecoder("/foo?a=&a=2");
    ASSERT_STREQ("/foo", d->getPath().c_str());

    params = &(d->getParameters());
    ASSERT_EQ(1, params->size());
    ASSERT_EQ(2, params->find("a")->second.size());
    ASSERT_STREQ("", params->find("a")->second.at(0).c_str());
    ASSERT_STREQ("2", params->find("a")->second.at(1).c_str());
    delete d;

    d = new QueryStringDecoder("/foo?a=1&a=");
    ASSERT_STREQ("/foo", d->getPath().c_str());

    params = &(d->getParameters());
    ASSERT_EQ(1, params->size());
    ASSERT_EQ(2, params->find("a")->second.size());
    ASSERT_STREQ("1", params->find("a")->second.at(0).c_str());
    ASSERT_STREQ("", params->find("a")->second.at(1).c_str());
    delete d;

    d = new QueryStringDecoder("/foo?a=1&a=&a=");
    ASSERT_STREQ("/foo", d->getPath().c_str());

    params = &(d->getParameters());
    ASSERT_EQ(1, params->size());
    ASSERT_EQ(3, params->find("a")->second.size());
    ASSERT_STREQ("1", params->find("a")->second.at(0).c_str());
    ASSERT_STREQ("", params->find("a")->second.at(1).c_str());
    ASSERT_STREQ("", params->find("a")->second.at(2).c_str());
    delete d;

    d = new QueryStringDecoder("/foo?a=1=&a==2");
    ASSERT_STREQ("/foo", d->getPath().c_str());

    params = &(d->getParameters());
    ASSERT_EQ(1, params->size());
    ASSERT_EQ(2, params->find("a")->second.size());
    ASSERT_STREQ("1=", params->find("a")->second.at(0).c_str());
    ASSERT_STREQ("=2", params->find("a")->second.at(1).c_str());
    delete d;
}

void assertQueryString(const char* expected, const char* actual) {
    QueryStringDecoder ed(expected);
    QueryStringDecoder ad(actual);
    ASSERT_STREQ(ed.getPath().c_str(), ad.getPath().c_str());
    ASSERT_TRUE(ed.getParameters() == ad.getParameters());
}

TEST(QueryStringDecoderTest, testExotic) {
    assertQueryString("", "");
    assertQueryString("foo", "foo");
    assertQueryString("/foo", "/foo");
    assertQueryString("?a=", "?a");
    assertQueryString("foo?a=", "foo?a");
    assertQueryString("/foo?a=", "/foo?a");
    assertQueryString("/foo?a=", "/foo?a&");
    assertQueryString("/foo?a=", "/foo?&a");
    assertQueryString("/foo?a=", "/foo?&a&");
    assertQueryString("/foo?a=", "/foo?&=a");
    assertQueryString("/foo?a=", "/foo?=a&");
    assertQueryString("/foo?a=", "/foo?a=&");
    assertQueryString("/foo?a=b&c=d", "/foo?a=b&&c=d");
    assertQueryString("/foo?a=b&c=d", "/foo?a=b&=&c=d");
    assertQueryString("/foo?a=b&c=d", "/foo?a=b&==&c=d");
    assertQueryString("/foo?a=b&c=&x=y", "/foo?a=b&c&x=y");
    assertQueryString("/foo?a=", "/foo?a=");
    assertQueryString("/foo?a=", "/foo?&a=");
    assertQueryString("/foo?a=b&c=d", "/foo?a=b&c=d");
    assertQueryString("/foo?a=1&a=&a=", "/foo?a=1&a&a=");
}

