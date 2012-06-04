/*
 * Copyright 2009 Red Hat, Inc.
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

#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/buffer/ChannelBufferIndexFinder.h"

using namespace cetty::buffer;

static const char str[] = "abc\r\n\ndef\r\rghi\n\njkl\0\0mno  \t\tx";

TEST(ChannelBufferIndexFinderTest, testForward) {
    ChannelBufferPtr buf = ChannelBuffers::copiedBuffer(Array((char*)str, 29));

    EXPECT_EQ(3, buf->indexOf(INT_MIN, buf->capacity(), ChannelBufferIndexFinder::CRLF));
    EXPECT_EQ(6, buf->indexOf(3, buf->capacity(), ChannelBufferIndexFinder::NOT_CRLF));
    EXPECT_EQ(9, buf->indexOf(6, buf->capacity(), ChannelBufferIndexFinder::CR));
    EXPECT_EQ(11, buf->indexOf(9, buf->capacity(), ChannelBufferIndexFinder::NOT_CR));
    EXPECT_EQ(14, buf->indexOf(11, buf->capacity(), ChannelBufferIndexFinder::LF));
    EXPECT_EQ(16, buf->indexOf(14, buf->capacity(), ChannelBufferIndexFinder::NOT_LF));
    EXPECT_EQ(19, buf->indexOf(16, buf->capacity(), ChannelBufferIndexFinder::NUL));
    EXPECT_EQ(21, buf->indexOf(19, buf->capacity(), ChannelBufferIndexFinder::NOT_NUL));
    EXPECT_EQ(24, buf->indexOf(21, buf->capacity(), ChannelBufferIndexFinder::LINEAR_WHITESPACE));
    EXPECT_EQ(28, buf->indexOf(24, buf->capacity(), ChannelBufferIndexFinder::NOT_LINEAR_WHITESPACE));
    EXPECT_EQ(-1, buf->indexOf(28, buf->capacity(), ChannelBufferIndexFinder::LINEAR_WHITESPACE));

}

TEST(ChannelBufferIndexFinderTest, testBackward) {
    ChannelBufferPtr buf = ChannelBuffers::copiedBuffer(Array((char*)str, 29));

    EXPECT_EQ(27, buf->indexOf(INT_MAX, 0, ChannelBufferIndexFinder::LINEAR_WHITESPACE));
    EXPECT_EQ(23, buf->indexOf(28, 0, ChannelBufferIndexFinder::NOT_LINEAR_WHITESPACE));
    EXPECT_EQ(20, buf->indexOf(24, 0, ChannelBufferIndexFinder::NUL));
    EXPECT_EQ(18, buf->indexOf(21, 0, ChannelBufferIndexFinder::NOT_NUL));
    EXPECT_EQ(15, buf->indexOf(19, 0, ChannelBufferIndexFinder::LF));
    EXPECT_EQ(13, buf->indexOf(16, 0, ChannelBufferIndexFinder::NOT_LF));
    EXPECT_EQ(10, buf->indexOf(14, 0, ChannelBufferIndexFinder::CR));
    EXPECT_EQ(8, buf->indexOf(11, 0, ChannelBufferIndexFinder::NOT_CR));
    EXPECT_EQ(5, buf->indexOf(9, 0, ChannelBufferIndexFinder::CRLF));
    EXPECT_EQ(2, buf->indexOf(6, 0, ChannelBufferIndexFinder::NOT_CRLF));
    EXPECT_EQ(-1, buf->indexOf(3, 0, ChannelBufferIndexFinder::CRLF));
}

