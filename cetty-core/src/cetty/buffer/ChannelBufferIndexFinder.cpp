/*
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

#include <cetty/buffer/ChannelBufferIndexFinder.h>

#include <boost/bind.hpp>
#include <cetty/buffer/ChannelBuffer.h>

namespace cetty {
namespace buffer {

/**
 * Index finder which locates a <tt>NUL (0x00)</tt> byte.
 */
static bool nullFinder(const ChannelBuffer& buffer, int guessedIndex) {
    return buffer.getByte(guessedIndex) == 0;
}

/**
 * Index finder which locates a non-<tt>NUL (0x00)</tt> byte.
 */
static bool notNulFinder(const ChannelBuffer& buffer, int guessedIndex) {
    return buffer.getByte(guessedIndex) != 0;
}

/**
 * Index finder which locates a <tt>CR ('\r')</tt> byte.
 */
static bool crFinder(const ChannelBuffer& buffer, int guessedIndex) {
    return buffer.getByte(guessedIndex) == '\r';
}

/**
 * Index finder which locates a non-<tt>CR ('\r')</tt> byte.
 */
static bool notCrFinder(const ChannelBuffer& buffer, int guessedIndex) {
    return buffer.getByte(guessedIndex) != '\r';
}

/**
 * Index finder which locates a <tt>LF ('\n')</tt> byte.
 */
static bool lfFinder(const ChannelBuffer& buffer, int guessedIndex) {
    return buffer.getByte(guessedIndex) == '\n';
}

/**
 * Index finder which locates a non-<tt>LF ('\n')</tt> byte.
 */
static bool notLfFinder(const ChannelBuffer& buffer, int guessedIndex) {
    return buffer.getByte(guessedIndex) != '\n';
}

/**
 * Index finder which locates a <tt>CR ('\r')</tt> or <tt>LF ('\n')</tt>.
 */
static bool crLfFinder(const ChannelBuffer& buffer, int guessedIndex) {
    int8_t b = buffer.getByte(guessedIndex);
    return b == '\r' || b == '\n';
}

/**
 * Index finder which locates a byte which is neither a <tt>CR ('\r')</tt>
 * nor a <tt>LF ('\n')</tt>.
 */
static bool notCrLfFinder(const ChannelBuffer& buffer, int guessedIndex) {
    int8_t b = buffer.getByte(guessedIndex);
    return b != '\r' && b != '\n';
}

/**
 * Index finder which locates a linear whitespace
 * (<tt>' '</tt> and <tt>'\t'</tt>).
 */
static bool linearWhitespaceFinder(const ChannelBuffer& buffer, int guessedIndex) {
    int8_t b = buffer.getByte(guessedIndex);
    return b == ' ' || b == '\t';
}

/**
 * Index finder which locates a byte which is not a linear whitespace
 * (neither <tt>' '</tt> nor <tt>'\t'</tt>).
 */
static bool notLinearWhitespaceFinder(const ChannelBuffer& buffer, int guessedIndex) {
    int8_t b = buffer.getByte(guessedIndex);
    return b != ' ' && b != '\t';
}

const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::NUL = boost::bind(nullFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::NOT_NUL = boost::bind(notNulFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::CR = boost::bind(crFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::NOT_CR = boost::bind(notCrFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::LF = boost::bind(lfFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::NOT_LF = boost::bind(notLfFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::CRLF = boost::bind(crLfFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::NOT_CRLF = boost::bind(notCrLfFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::LINEAR_WHITESPACE = boost::bind(linearWhitespaceFinder, _1, _2);
const ChannelBufferIndexFinder::Functor ChannelBufferIndexFinder::NOT_LINEAR_WHITESPACE = boost::bind(notLinearWhitespaceFinder, _1, _2);

}
}
