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

#include <cetty/util/Base64.h>
#include <boost/assert.hpp>

namespace cetty {
namespace util {

/**
 * Lookup table for turning bytes into their position in the
 * Base64 alphabet.
 */
static const int DECODE[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

/**
 * Decode lookup table for the "web safe" variant (RFC 3548
 * sec. 4) where - and _ replace + and /.
 */
static const int DECODE_WEBSAFE[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -2, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

/**
 * Lookup table for turning Base64 alphabet positions (6 bits)
 * into output bytes.
 */
static const char ENCODE[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
};

/**
 * Lookup table for turning Base64 alphabet positions (6 bits)
 * into output bytes.
 */
static const char ENCODE_WEBSAFE[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_',
};

/**
 * Emit a new line every this many output tuples.  Corresponds to
 * a 76-character line length (the maximum allowable according to
 * <a href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045</a>).
 */
static const int LINE_GROUPS = 19;

bool Base64::decode(const char* str, int size, int flags, std::string* bytes) {
    /** Non-data values in the DECODE arrays. */
    static const int SKIP = -1;
    static const int EQUALS = -2;

    if (NULL == str || size <= 0 || NULL == bytes) {
        return false;
    }

    /**
     * States 0-3 are reading through the next input tuple.
     * State 4 is having read one '=' and expecting exactly
     * one more.
     * State 5 is expecting no more data or padding characters
     * in the input.
     * State 6 is the error state; an error has been detected
     * in the input and no future input can "fix" it.
     */
    int state = 0;

    int p = 0;
    int op = 0;

    bytes->reserve(size * 3 / 4);
    char* output = (char*)bytes->data();

    const int* alphabet = 
        (const int*)(((flags & URL_SAFE) == 0) ? DECODE : DECODE_WEBSAFE);

    int value;
    while (p < size) {
        // Try the fast path:  we're starting a new tuple and the
        // next four bytes of the input stream are all data
        // bytes.  This corresponds to going through states
        // 0-1-2-3-0.  We expect to use this method for most of
        // the data.
        //
        // If any of the next four bytes of input are non-data
        // (whitespace, etc.), value will end up negative.  (All
        // the non-data values in decode are small negative
        // numbers, so shifting any of them up and or'ing them
        // together will result in a value with its top bit set.)
        //
        // You can remove this whole block and the output should
        // be the same, just slower.
        if (state == 0) {
            while (p+4 <= size &&
                    (value = ((alphabet[str[p] & 0xff] << 18) |
                              (alphabet[str[p+1] & 0xff] << 12) |
                              (alphabet[str[p+2] & 0xff] << 6) |
                              (alphabet[str[p+3] & 0xff]))) >= 0) {
                output[op+2] = (char) value;
                output[op+1] = (char)(value >> 8);
                output[op]   = (char)(value >> 16);
                op += 3;
                p += 4;
            }

            if (p >= size) { break; }
        }

        // The fast path isn't available -- either we've read a
        // partial tuple, or the next four input bytes aren't all
        // data, or whatever.  Fall back to the slower state
        // machine implementation.

        int d = alphabet[str[p++] & 0xff];

        switch (state) {
        case 0:
            if (d >= 0) {
                value = d;
                ++state;
            }
            else if (d != SKIP) {
                //state = 6;
                return false;
            }

            break;

        case 1:
            if (d >= 0) {
                value = (value << 6) | d;
                ++state;
            }
            else if (d != SKIP) {
                //state = 6;
                return false;
            }

            break;

        case 2:
            if (d >= 0) {
                value = (value << 6) | d;
                ++state;
            }
            else if (d == EQUALS) {
                // Emit the last (partial) output tuple;
                // expect exactly one more padding character.
                output[op++] = (char)(value >> 4);
                state = 4;
            }
            else if (d != SKIP) {
                //state = 6;
                return false;
            }

            break;

        case 3:
            if (d >= 0) {
                // Emit the output triple and return to state 0.
                value = (value << 6) | d;
                output[op+2] = (char) value;
                output[op+1] = (char)(value >> 8);
                output[op] = (char)(value >> 16);
                op += 3;
                state = 0;
            }
            else if (d == EQUALS) {
                // Emit the last (partial) output tuple;
                // expect no further data or padding characters.
                output[op+1] = (char)(value >> 2);
                output[op] = (char)(value >> 10);
                op += 2;
                state = 5;
            }
            else if (d != SKIP) {
                //state = 6;
                return false;
            }

            break;

        case 4:
            if (d == EQUALS) {
                ++state;
            }
            else if (d != SKIP) {
                //state = 6;
                return false;
            }

            break;

        case 5:
            if (d != SKIP) {
                //state = 6;
                return false;
            }

            break;
        }
    }

    bytes->resize(op);
    return true;
}

bool Base64::encode(const char* bytes, int size, int flags, std::string* str) {
    bool doPadding = (flags & NO_PADDING) == 0;
    bool doNewline = (flags & NO_WRAP) == 0;
    bool doCr = (flags & CRLF) != 0;
    const char* alphabet = ((flags & URL_SAFE) == 0) ? ENCODE : ENCODE_WEBSAFE;

    int count = doNewline ? LINE_GROUPS : -1;

    // Compute the exact length of the array we will produce.
    int outputLen = size / 3 * 4;

    // Account for the tail of the data and the padding bytes, if any.
    if (doPadding) {
        if (size % 3 > 0) {
            outputLen += 4;
        }
    }
    else {
        switch (size % 3) {
        case 0: break;

        case 1: outputLen += 2; break;

        case 2: outputLen += 3; break;
        }
    }

    // Account for the newlines, if any.
    if (doNewline && size > 0) {
        outputLen += (((size-1) / (3 * LINE_GROUPS)) + 1) * (doCr ? 2 : 1);
    }

    str->reserve(outputLen);
    char* output = (char*)str->data();

    int p = 0;
    int op = 0;
    int v = -1;

    // The main loop, turning 3 input bytes into 4 output bytes on
    // each iteration.
    while (p+3 <= size) {
        v = ((bytes[p]   & 0xff) << 16) |
            ((bytes[p+1] & 0xff) <<  8) |
             (bytes[p+2] & 0xff);
        output[op++] = alphabet[(v >> 18) & 0x3f];
        output[op++] = alphabet[(v >> 12) & 0x3f];
        output[op++] = alphabet[(v >>  6) & 0x3f];
        output[op++] = alphabet[v & 0x3f];
        p += 3;
        op += 4;

        if (--count == 0) {
            if (doCr) { (*str)[op++] = '\r'; }

            (*str)[op++] = '\n';
            count = LINE_GROUPS;
        }
    }

    // Finish up the tail of the input.  Note that we need to
    // consume any bytes in tail before any bytes
    // remaining in input; there should be at most two bytes
    // total.

    if (p == size-1) {
        v = (bytes[p++] & 0xff) << 4;
        output[op++] = alphabet[(v >> 6) & 0x3f];
        output[op++] = alphabet[v & 0x3f];

        if (doPadding) {
            output[op++] = '=';
            output[op++] = '=';
        }

        if (doNewline) {
            if (doCr) { output[op++] = '\r'; }

            output[op++] = '\n';
        }
    }
    else if (p == size-2) {
        v |= ((bytes[p++] & 0xff) << 10);
        v |= ((bytes[p++] & 0xff) << 2);

        output[op++] = alphabet[(v >> 12) & 0x3f];
        output[op++] = alphabet[(v >> 6) & 0x3f];
        output[op++] = alphabet[v & 0x3f];

        if (doPadding) {
            output[op++] = '=';
        }

        if (doNewline) {
            if (doCr) { output[op++] = '\r'; }

            output[op++] = '\n';
        }
    }
    else if (doNewline && op > 0 && count != LINE_GROUPS) {
        if (doCr) { output[op++] = '\r'; }

        output[op++] = '\n';
    }

    BOOST_ASSERT(p == size);
    return true;
}

}
}
