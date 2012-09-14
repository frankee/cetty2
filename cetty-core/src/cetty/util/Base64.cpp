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


void Base64::decode(const char* str, int size, int flags, std::string* bytes) {

}

void Base64::encode(const char* bytes, int size, int flags, std::string* str) {
#if 0
     bool do_padding = (flags & NO_PADDING) == 0;
     bool do_newline  = (flags & NO_WRAP) == 0;
     bool do_cr  = (flags & CRLF) != 0;
     const char* alphabet = ((flags & URL_SAFE) == 0) ? ENCODE : ENCODE_WEBSAFE;

     char tail[2];
     int tailLen = 0;
     int count = do_newline ? LINE_GROUPS : -1;

    // Using local variables makes the encoder about 9% faster.
    int op = 0;
    int count = this.count;

    int p = offset;
    len += offset;
    int v = -1;

    // First we need to concatenate the tail of the previous call
    // with any input bytes available now and see if we can empty
    // the tail.

    switch (tailLen) {
    case 0:
        // There was no tail.
        break;

    case 1:
        if (p+2 <= len) {
            // A 1-byte tail with at least 2 bytes of
            // input available now.
            v = ((tail[0] & 0xff) << 16) |
                ((input[p++] & 0xff) << 8) |
                (input[p++] & 0xff);
            tailLen = 0;
        };
        break;

    case 2:
        if (p+1 <= len) {
            // A 2-byte tail with at least 1 byte of input.
            v = ((tail[0] & 0xff) << 16) |
                ((tail[1] & 0xff) << 8) |
                (input[p++] & 0xff);
            tailLen = 0;
        }
        break;
    }

    if (v != -1) {
        output[op++] = alphabet[(v >> 18) & 0x3f];
        output[op++] = alphabet[(v >> 12) & 0x3f];
        output[op++] = alphabet[(v >> 6) & 0x3f];
        output[op++] = alphabet[v & 0x3f];
        if (--count == 0) {
            if (do_cr) output[op++] = '\r';
            output[op++] = '\n';
            count = LINE_GROUPS;
        }
    }

    // At this point either there is no tail, or there are fewer
    // than 3 bytes of input available.

    // The main loop, turning 3 input bytes into 4 output bytes on
    // each iteration.
    while (p+3 <= len) {
        v = ((input[p] & 0xff) << 16) |
            ((input[p+1] & 0xff) << 8) |
            (input[p+2] & 0xff);
        output[op] = alphabet[(v >> 18) & 0x3f];
        output[op+1] = alphabet[(v >> 12) & 0x3f];
        output[op+2] = alphabet[(v >> 6) & 0x3f];
        output[op+3] = alphabet[v & 0x3f];
        p += 3;
        op += 4;
        if (--count == 0) {
            if (do_cr) output[op++] = '\r';
            output[op++] = '\n';
            count = LINE_GROUPS;
        }
    }

    if (finish) {
        // Finish up the tail of the input.  Note that we need to
        // consume any bytes in tail before any bytes
        // remaining in input; there should be at most two bytes
        // total.

        if (p-tailLen == len-1) {
            int t = 0;
            v = ((tailLen > 0 ? tail[t++] : input[p++]) & 0xff) << 4;
            tailLen -= t;
            output[op++] = alphabet[(v >> 6) & 0x3f];
            output[op++] = alphabet[v & 0x3f];
            if (do_padding) {
                output[op++] = '=';
                output[op++] = '=';
            }
            if (do_newline) {
                if (do_cr) output[op++] = '\r';
                output[op++] = '\n';
            }
        } else if (p-tailLen == len-2) {
            int t = 0;
            v = (((tailLen > 1 ? tail[t++] : input[p++]) & 0xff) << 10) |
                (((tailLen > 0 ? tail[t++] : input[p++]) & 0xff) << 2);
            tailLen -= t;
            output[op++] = alphabet[(v >> 12) & 0x3f];
            output[op++] = alphabet[(v >> 6) & 0x3f];
            output[op++] = alphabet[v & 0x3f];
            if (do_padding) {
                output[op++] = '=';
            }
            if (do_newline) {
                if (do_cr) output[op++] = '\r';
                output[op++] = '\n';
            }
        } else if (do_newline && op > 0 && count != LINE_GROUPS) {
            if (do_cr) output[op++] = '\r';
            output[op++] = '\n';
        }

        assert tailLen == 0;
        assert p == len;
    } else {
        // Save the leftovers in tail to be consumed on the next
        // call to encodeInternal.

        if (p == len-1) {
            tail[tailLen++] = input[p];
        } else if (p == len-2) {
            tail[tailLen++] = input[p];
            tail[tailLen++] = input[p+1];
        }
    }

    this.op = op;
    this.count = count;

    return true;

#endif
}

}
}
