/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <gtest/gtest.h>
#include <boost/date_time.hpp>
#include <boost/algorithm/string.hpp>
#include <cetty/handler/codec/http/Cookie.h>
#include <cetty/handler/codec/http/ClientCookieEncoder.h>
#include <cetty/handler/codec/http/ServerCookieEncoder.h>
#include <cetty/handler/codec/http/HttpHeaderDateFormat.h>

using namespace boost::posix_time;
using namespace cetty::handler::codec::http;

TEST(CookieEncoderTest, testEncodingSingleCookieV0) {
    std::string result = "myCookie=myValue; Expires=XXX; Path=/apathsomewhere; Domain=.adomainsomewhere; Secure";

    Cookie cookie("myCookie", "myValue");
    cookie.setComment("this is a Comment");
    cookie.setCommentUrl("http://aurl.com");
    cookie.setDomain(".adomainsomewhere");
    cookie.setDiscard(true);
    cookie.setMaxAge(50);
    cookie.setPath("/apathsomewhere");
    cookie.setPorts(80, 8080);
    cookie.setSecure(true);

    std::string encodedCookie;
    ServerCookieEncoder::encode(cookie, &encodedCookie);

    ptime currentTime = microsec_clock::universal_time();
    bool fail = true;

    // +/- 10-second tolerance
    for (int delta = 0; delta <= 20000; delta += 250) {
        ptime time = currentTime + milliseconds(40000) + milliseconds(delta);
        boost::replace_first(result,
                             "XXX",
                             HttpHeaderDateFormat::cookieFormat.format(time));

        if (encodedCookie == result) {
            fail = false;
            break;
        }
    }

    if (fail) {
        fail("Expected: " + result + ", Actual: " + encodedCookie);
    }
}

TEST(CookieEncoderTest, testEncodingSingleCookieV1)  {
    std::string result = "myCookie=myValue; Max-Age=50; Path=\"/apathsomewhere\"; Domain=.adomainsomewhere; Secure; Comment=\"this is a Comment\"; Version=1";
    Cookie cookie("myCookie", "myValue");
    cookie.setVersion(1);
    cookie.setComment("this is a Comment");
    cookie.setDomain(".adomainsomewhere");
    cookie.setMaxAge(50);
    cookie.setPath("/apathsomewhere");
    cookie.setSecure(true);
    
    std::string encodedCookie = ServerCookieEncoder::encode(cookie);
    ASSERT_STREQ(result.c_str(), encodedCookie.c_str());
}

TEST(CookieEncoderTest, testEncodingSingleCookieV2)  {
    std::string result = "myCookie=myValue; Max-Age=50; Path=\"/apathsomewhere\"; Domain=.adomainsomewhere; Secure; Comment=\"this is a Comment\"; Version=1; CommentURL=\"http://aurl.com\"; Port=\"80,8080\"; Discard";
    Cookie cookie("myCookie", "myValue");
    cookie.setVersion(1);
    cookie.setComment("this is a Comment");
    cookie.setCommentUrl("http://aurl.com");
    cookie.setDomain(".adomainsomewhere");
    cookie.setDiscard(true);
    cookie.setMaxAge(50);
    cookie.setPath("/apathsomewhere");
    cookie.setPorts(80, 8080);
    cookie.setSecure(true);

    std::string encodedCookie = ServerCookieEncoder::encode(cookie);
    ASSERT_STREQ(result.c_str(), encodedCookie.c_str());
}

TEST(CookieEncoderTest, testEncodingSingleCookieV2) {
    std::string c1 = "$Version=1; myCookie=myValue; $Path=\"/apathsomewhere\"; $Domain=.adomainsomewhere; $Port=\"80,8080\"; ";
    std::string c2 = "$Version=1; myCookie2=myValue2; $Path=\"/anotherpathsomewhere\"; $Domain=.anotherdomainsomewhere; ";
    std::string c3 = "$Version=1; myCookie3=myValue3";

    Cookie cookie("myCookie", "myValue");
    cookie.setVersion(1);
    cookie.setComment("this is a Comment");
    cookie.setCommentUrl("http://aurl.com");
    cookie.setDomain(".adomainsomewhere");
    cookie.setDiscard(true);
    cookie.setMaxAge(50);
    cookie.setPath("/apathsomewhere");
    cookie.setPorts(80, 8080);
    cookie.setSecure(true);

    Cookie cookie2("myCookie2", "myValue2");
    cookie2.setVersion(1);
    cookie2.setComment("this is another Comment");
    cookie2.setCommentUrl("http://anotherurl.com");
    cookie2.setDomain(".anotherdomainsomewhere");
    cookie2.setDiscard(false);
    cookie2.setPath("/anotherpathsomewhere");
    cookie2.setSecure(false);

    Cookie cookie3("myCookie3", "myValue3");
    cookie3.setVersion(1);

    std::string c(c1);
    c += c2;
    c += c3;

    std::string encodedCookie = ClientCookieEncoder::encode(cookie, cookie2, cookie3);
    ASSERT_STREQ(c.c_str(), encodedCookie.c_str());
}


