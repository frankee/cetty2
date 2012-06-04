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
#include "gtest/gtest.h"
#include "cetty/handler/codec/http/HttpMethod.h"
#include "cetty/handler/codec/http/HttpVersion.h"
#include "cetty/handler/codec/http/DefaultHttpMessage.h"
#include "cetty/handler/codec/http/DefaultHttpRequest.h"
#include "cetty/util/Integer.h"

using namespace cetty::util;
using namespace cetty::handler::codec::http;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2260 $, $Date: 2010-05-04 10:48:52 +0900 (Tue, 04 May 2010) $
 */
TEST(DefaultHttpRequestTest, testHeaderRemoval) {
    DefaultHttpRequest m(HttpVersion::HTTP_1_1, HttpMethod::HM_GET, "/");

    // Insert sample keys.
    for (int i = 0; i < 1000; ++i) {
        m.setHeader(Integer::toString(i), "");
    }

    // Remove in reversed order.
    for (int i = 999; i >= 0; --i) {
        m.removeHeader(Integer::toString(i));
    }

    // Check if random access returns nothing.
    for (int i = 0; i < 1000; ++i) {
        ASSERT_TRUE(m.getHeader(Integer::toString(i)).empty());
    }

    // Check if sequential access returns nothing.
    ASSERT_TRUE(m.getHeaders().empty());
}
