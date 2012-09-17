/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "gtest/gtest.h"
#include "cetty/util/Base64::h"

using namespace cetty::util;

/** Decodes a string, returning a string. */
std::string decodeString(const std::string& in) {
    std::string out;
    Base64::decode(in, &out);
    return out;
}

/**
     * Encodes the string 'in' using 'flags'.  Asserts that decoding
     * gives the same string.  Returns the encoded string.
     */
std::string encodeToString(const std::string& in, int flags) {
    std::string b64;
    Base64::encode(in, flags, &b64);

    ASSERT_STREQ(b64, decodeString(b64));
    return b64;
}

/** Assert that decoding 'in' throws IllegalArgumentException. */
void assertBad(const std::string& in) {
    std::string out;
    ASSERT_FALSE(Base64::decode(in, &out));
}

TEST(Base64Test, testDecodeExtraChars) {
    // padding 0
    ASSERT_STREQ("hello, world", decodeString("aGVsbG8sIHdvcmxk"));
    assertBad("aGVsbG8sIHdvcmxk=");
    assertBad("aGVsbG8sIHdvcmxk==");
    assertBad("aGVsbG8sIHdvcmxk =");
    assertBad("aGVsbG8sIHdvcmxk = = ");
    ASSERT_STREQ("hello, world", decodeString(" aGVs bG8s IHdv cmxk  "));
    ASSERT_STREQ("hello, world", decodeString(" aGV sbG8 sIHd vcmx k "));
    ASSERT_STREQ("hello, world", decodeString(" aG VsbG 8sIH dvcm xk "));
    ASSERT_STREQ("hello, world", decodeString(" a GVsb G8sI Hdvc mxk "));
    ASSERT_STREQ("hello, world", decodeString(" a G V s b G 8 s I H d v c m x k "));
    ASSERT_STREQ("hello, world", decodeString("_a*G_V*s_b*G_8*s_I*H_d*v_c*m_x*k_"));
    ASSERT_STREQ("hello, world", decodeString("aGVsbG8sIHdvcmxk"));

    // padding 1
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPyE="));
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPyE"));
    assertBad("aGVsbG8sIHdvcmxkPyE==");
    assertBad("aGVsbG8sIHdvcmxkPyE ==");
    assertBad("aGVsbG8sIHdvcmxkPyE = = ");
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPy E="));
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPy E"));
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPy E ="));
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPy E "));
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPy E = "));
    ASSERT_STREQ("hello, world?!", decodeString("aGVsbG8sIHdvcmxkPy E   "));

    // padding 2
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkLg=="));
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkLg"));
    assertBad("aGVsbG8sIHdvcmxkLg=");
    assertBad("aGVsbG8sIHdvcmxkLg =");
    assertBad("aGVsbG8sIHdvcmxkLg = ");
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkL g=="));
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkL g"));
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkL g =="));
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkL g "));
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkL g = = "));
    ASSERT_STREQ("hello, world.", decodeString("aGVsbG8sIHdvcmxkL g   "));
}

static const char BYTES[] = {
    (char) 0xff, (char) 0xee, (char) 0xdd,
    (char) 0xcc, (char) 0xbb, (char) 0xaa,
    (char) 0x99, (char) 0x88, (char) 0x77
};

TEST(Base64Test, testBinaryDecode)  {
    ASSERT_STREQ(BYTES, 0, Base64::decode("", 0));
    ASSERT_STREQ(BYTES, 1, Base64::decode("/w==", 0));
    ASSERT_STREQ(BYTES, 2, Base64::decode("/+4=", 0));
    ASSERT_STREQ(BYTES, 3, Base64::decode("/+7d", 0));
    ASSERT_STREQ(BYTES, 4, Base64::decode("/+7dzA==", 0));
    ASSERT_STREQ(BYTES, 5, Base64::decode("/+7dzLs=", 0));
    ASSERT_STREQ(BYTES, 6, Base64::decode("/+7dzLuq", 0));
    ASSERT_STREQ(BYTES, 7, Base64::decode("/+7dzLuqmQ==", 0));
    ASSERT_STREQ(BYTES, 8, Base64::decode("/+7dzLuqmYg=", 0));
}

TEST(Base64Test, testWebSafe) {
    ASSERT_STREQ(BYTES, 0, Base64::decode("", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 1, Base64::decode("_w==", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 2, Base64::decode("_-4=", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 3, Base64::decode("_-7d", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 4, Base64::decode("_-7dzA==", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 5, Base64::decode("_-7dzLs=", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 6, Base64::decode("_-7dzLuq", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 7, Base64::decode("_-7dzLuqmQ==", Base64::URL_SAFE));
    ASSERT_STREQ(BYTES, 8, Base64::decode("_-7dzLuqmYg=", Base64::URL_SAFE));

    ASSERT_STREQ("", Base64::encodeToString(BYTES, 0, 0, Base64::URL_SAFE));
    ASSERT_STREQ("_w==\n", Base64::encodeToString(BYTES, 0, 1, Base64::URL_SAFE));
    ASSERT_STREQ("_-4=\n", Base64::encodeToString(BYTES, 0, 2, Base64::URL_SAFE));
    ASSERT_STREQ("_-7d\n", Base64::encodeToString(BYTES, 0, 3, Base64::URL_SAFE));
    ASSERT_STREQ("_-7dzA==\n", Base64::encodeToString(BYTES, 0, 4, Base64::URL_SAFE));
    ASSERT_STREQ("_-7dzLs=\n", Base64::encodeToString(BYTES, 0, 5, Base64::URL_SAFE));
    ASSERT_STREQ("_-7dzLuq\n", Base64::encodeToString(BYTES, 0, 6, Base64::URL_SAFE));
    ASSERT_STREQ("_-7dzLuqmQ==\n", Base64::encodeToString(BYTES, 0, 7, Base64::URL_SAFE));
    ASSERT_STREQ("_-7dzLuqmYg=\n", Base64::encodeToString(BYTES, 0, 8, Base64::URL_SAFE));
}

TEST(Base64Test, testFlags) {
    ASSERT_STREQ("YQ==\n",       encodeToString("a", 0));
    ASSERT_STREQ("YQ==",         encodeToString("a", Base64::NO_WRAP));
    ASSERT_STREQ("YQ\n",         encodeToString("a", Base64::NO_PADDING));
    ASSERT_STREQ("YQ",           encodeToString("a", Base64::NO_PADDING | Base64::NO_WRAP));
    ASSERT_STREQ("YQ==\r\n",     encodeToString("a", Base64::CRLF));
    ASSERT_STREQ("YQ\r\n",       encodeToString("a", Base64::CRLF | Base64::NO_PADDING));

    ASSERT_STREQ("YWI=\n",       encodeToString("ab", 0));
    ASSERT_STREQ("YWI=",         encodeToString("ab", Base64::NO_WRAP));
    ASSERT_STREQ("YWI\n",        encodeToString("ab", Base64::NO_PADDING));
    ASSERT_STREQ("YWI",          encodeToString("ab", Base64::NO_PADDING | Base64::NO_WRAP));
    ASSERT_STREQ("YWI=\r\n",     encodeToString("ab", Base64::CRLF));
    ASSERT_STREQ("YWI\r\n",      encodeToString("ab", Base64::CRLF | Base64::NO_PADDING));

    ASSERT_STREQ("YWJj\n",       encodeToString("abc", 0));
    ASSERT_STREQ("YWJj",         encodeToString("abc", Base64::NO_WRAP));
    ASSERT_STREQ("YWJj\n",       encodeToString("abc", Base64::NO_PADDING));
    ASSERT_STREQ("YWJj",         encodeToString("abc", Base64::NO_PADDING | Base64::NO_WRAP));
    ASSERT_STREQ("YWJj\r\n",     encodeToString("abc", Base64::CRLF));
    ASSERT_STREQ("YWJj\r\n",     encodeToString("abc", Base64::CRLF | Base64::NO_PADDING));

    ASSERT_STREQ("YWJjZA==\n",   encodeToString("abcd", 0));
    ASSERT_STREQ("YWJjZA==",     encodeToString("abcd", Base64::NO_WRAP));
    ASSERT_STREQ("YWJjZA\n",     encodeToString("abcd", Base64::NO_PADDING));
    ASSERT_STREQ("YWJjZA",       encodeToString("abcd", Base64::NO_PADDING | Base64::NO_WRAP));
    ASSERT_STREQ("YWJjZA==\r\n", encodeToString("abcd", Base64::CRLF));
    ASSERT_STREQ("YWJjZA\r\n",   encodeToString("abcd", Base64::CRLF | Base64::NO_PADDING));
}

TEST(Base64Test, testLineLength) {
    String in_56 = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcd";
    String in_57 = in_56 + "e";
    String in_58 = in_56 + "ef";
    String in_59 = in_56 + "efg";
    String in_60 = in_56 + "efgh";
    String in_61 = in_56 + "efghi";

    String prefix = "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5emFi";
    String out_56 = prefix + "Y2Q=\n";
    String out_57 = prefix + "Y2Rl\n";
    String out_58 = prefix + "Y2Rl\nZg==\n";
    String out_59 = prefix + "Y2Rl\nZmc=\n";
    String out_60 = prefix + "Y2Rl\nZmdo\n";
    String out_61 = prefix + "Y2Rl\nZmdoaQ==\n";

    // no newline for an empty input array.
    ASSERT_STREQ("", encodeToString("", 0));

    ASSERT_STREQ(out_56, encodeToString(in_56, 0));
    ASSERT_STREQ(out_57, encodeToString(in_57, 0));
    ASSERT_STREQ(out_58, encodeToString(in_58, 0));
    ASSERT_STREQ(out_59, encodeToString(in_59, 0));
    ASSERT_STREQ(out_60, encodeToString(in_60, 0));
    ASSERT_STREQ(out_61, encodeToString(in_61, 0));

    ASSERT_STREQ(out_56.replaceAll("=", ""), encodeToString(in_56, Base64::NO_PADDING));
    ASSERT_STREQ(out_57.replaceAll("=", ""), encodeToString(in_57, Base64::NO_PADDING));
    ASSERT_STREQ(out_58.replaceAll("=", ""), encodeToString(in_58, Base64::NO_PADDING));
    ASSERT_STREQ(out_59.replaceAll("=", ""), encodeToString(in_59, Base64::NO_PADDING));
    ASSERT_STREQ(out_60.replaceAll("=", ""), encodeToString(in_60, Base64::NO_PADDING));
    ASSERT_STREQ(out_61.replaceAll("=", ""), encodeToString(in_61, Base64::NO_PADDING));

    ASSERT_STREQ(out_56.replaceAll("\n", ""), encodeToString(in_56, Base64::NO_WRAP));
    ASSERT_STREQ(out_57.replaceAll("\n", ""), encodeToString(in_57, Base64::NO_WRAP));
    ASSERT_STREQ(out_58.replaceAll("\n", ""), encodeToString(in_58, Base64::NO_WRAP));
    ASSERT_STREQ(out_59.replaceAll("\n", ""), encodeToString(in_59, Base64::NO_WRAP));
    ASSERT_STREQ(out_60.replaceAll("\n", ""), encodeToString(in_60, Base64::NO_WRAP));
    ASSERT_STREQ(out_61.replaceAll("\n", ""), encodeToString(in_61, Base64::NO_WRAP));
}
