#if !defined(CETTY_SHIRO_CODEC_BASE64_H)
#define CETTY_SHIRO_CODEC_BASE64_H
/*
* Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

namespace cetty {
namespace shiro {
namespace codec {

/**
 * Provides <a href="http://en.wikipedia.org/wiki/Base64">Base 64</a> encoding and decoding as defined by
 * <a href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045</a>.
 * <p/>
 * This class implements section <cite>6.8. Base64 Content-Transfer-Encoding</cite> from RFC 2045 <cite>Multipurpose
 * Internet Mail Extensions (MIME) Part One: Format of Internet Message Bodies</cite> by Freed and Borenstein.
 * <p/>
 * This class was borrowed from Apache Commons Codec SVN repository (rev. 618419) with modifications
 * to enable Base64 conversion without a full dependecny on Commons Codec.  We didn't want to reinvent the wheel of
 * great work they've done, but also didn't want to force every Shiro user to depend on the commons-codec.jar
 * <p/>
 * As per the Apache 2.0 license, the original copyright notice and all author and copyright information have
 * remained in tact.
 *
 * @see <a href="http://en.wikipedia.org/wiki/Base64">Wikipedia: Base 64</a>
 * @see <a href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045</a>
 * @since 0.9
 */
class Base64 {
private:
    /**
     * Chunk size per RFC 2045 section 6.8.
     * <p/>
     * The character limit does not count the trailing CRLF, but counts all other characters, including any
     * equal signs.
     *
     * @see <a href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045 section 6.8</a>
     */
    static const int CHUNK_SIZE = 76;

    /**
     * Chunk separator per RFC 2045 section 2.1.
     *
     * @see <a href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045 section 2.1</a>
     */
    static const std::string CHUNK_SEPARATOR;

    /**
     * The base length.
     */
    static const int BASELENGTH = 255;

    /**
     * Lookup length.
     */
    static const int LOOKUPLENGTH = 64;

    /**
     * Used to calculate the number of bits in a byte.
     */
    static const int EIGHTBIT = 8;

    /**
     * Used when encoding something which has fewer than 24 bits.
     */
    static const int SIXTEENBIT = 16;

    /**
     * Used to determine how many bits data contains.
     */
    static const int TWENTYFOURBITGROUP = 24;

    /**
     * Used to get the number of Quadruples.
     */
    static const int FOURBYTE = 4;

    /**
     * Used to test the sign of a byte.
     */
    static const int SIGN = -128;

    /**
     * Byte used to pad output.
     */
    static const std::string PAD;

    /**
     * Contains the Base64 values <code>0</code> through <code>63</code> accessed by using character encodings as
     * indices.
     * <p/>
     * <p>For example, <code>base64Alphabet['+']</code> returns <code>62</code>.</p>
     * <p/>
     * <p>The value of undefined encodings is <code>-1</code>.</p>
     */
    static char base64Alphabet[BASELENGTH];

    /**
     * <p>Contains the Base64 encodings <code>A</code> through <code>Z</code>, followed by <code>a</code> through
     * <code>z</code>, followed by <code>0</code> through <code>9</code>, followed by <code>+</code>, and
     * <code>/</code>.</p>
     * <p/>
     * <p>This array is accessed by using character values as indices.</p>
     * <p/>
     * <p>For example, <code>lookUpBase64Alphabet[62] </code> returns <code>'+'</code>.</p>
     */
    static char lookUpBase64Alphabet[LOOKUPLENGTH];

    // Populating the lookup and character arrays
/*
    static {
        for (int i = 0; i < BASELENGTH; i++) {
            base64Alphabet[i] = -1;
        }
        for (int i = 'Z'; i >= 'A'; i--) {
            base64Alphabet[i] = (byte) (i - 'A');
        }
        for (int i = 'z'; i >= 'a'; i--) {
            base64Alphabet[i] = (byte) (i - 'a' + 26);
        }
        for (int i = '9'; i >= '0'; i--) {
            base64Alphabet[i] = (byte) (i - '0' + 52);
        }

        base64Alphabet['+'] = 62;
        base64Alphabet['/'] = 63;

        for (int i = 0; i <= 25; i++) {
            lookUpBase64Alphabet[i] = (byte) ('A' + i);
        }

        for (int i = 26, j = 0; i <= 51; i++, j++) {
            lookUpBase64Alphabet[i] = (byte) ('a' + j);
        }

        for (int i = 52, j = 0; i <= 61; i++, j++) {
            lookUpBase64Alphabet[i] = (byte) ('0' + j);
        }

        lookUpBase64Alphabet[62] = (byte) '+';
        lookUpBase64Alphabet[63] = (byte) '/';
    }
*/

    /**
     * Returns whether or not the <code>octect</code> is in the base 64 alphabet.
     *
     * @param octect The value to test
     * @return <code>true</code> if the value is defined in the the base 64 alphabet, <code>false</code> otherwise.
     */
    static bool isBase64(char octect);

    /**
     * Tests a given byte array to see if it contains only valid characters within the Base64 alphabet.
     *
     * @param arrayOctect byte array to test
     * @return <code>true</code> if all bytes are valid characters in the Base64 alphabet or if the byte array is
     *         empty; false, otherwise
     */
public:
    static bool isBase64(const std::string &arrayOctect);

    /**
     * Discards any whitespace from a base-64 encoded block.
     *
     * @param data The base-64 encoded data to discard the whitespace from.
     * @return The data, less whitespace (see RFC 2045).
     */
    static std::string discardWhitespace(const std::string &data);
    /**
     * Base64 encodes the specified byte array and then encodes it as a String using Shiro's preferred character
     * encoding (UTF-8).
     *
     * @param bytes the byte array to Base64 encode.
     * @return a UTF-8 encoded String of the resulting Base64 encoded byte array.
     */
    static std::string encodeToString(const std::string & bytes);

    /**
     * Encodes binary data using the base64 algorithm and chunks the encoded output into 76 character blocks
     *
     * @param binaryData binary data to encodeToChars
     * @return Base64 characters chunked in 76 character blocks
     */
    static std::string encodeChunked(const std::string &binaryData) {
        return encode(binaryData, true);
    }

    /**
     * Encodes a byte[] containing binary data, into a byte[] containing characters in the Base64 alphabet.
     *
     * @param pArray a byte array containing binary data
     * @return A byte array containing only Base64 character data
     */
    static std::string encode(const std::string &pArray) {
        return encode(pArray, false);
    }

    /**
     * Encodes binary data using the base64 algorithm, optionally chunking the output into 76 character blocks.
     *
     * @param binaryData Array containing binary data to encodeToChars.
     * @param isChunked  if <code>true</code> this encoder will chunk the base64 output into 76 character blocks
     * @return Base64-encoded data.
     * @throws IllegalArgumentException Thrown when the input array needs an output array bigger than {@link Integer#MAX_VALUE}
     */
    static std::string encode(const std::string &binaryData, bool isChunked);
    /**
     * Converts the specified UTF-8 Base64 encoded String and decodes it to a resultant UTF-8 encoded string.
     *
     * @param base64Encoded a UTF-8 Base64 encoded String
     * @return the decoded String, UTF-8 encoded.
     */
    static std::string decodeToString(const std::string &base64Encoded);

    /**
     * Converts the specified UTF-8 Base64 encoded String and decodes it to a raw Base64 decoded byte array.
     *
     * @param base64Encoded a UTF-8 Base64 encoded String
     * @return the raw Base64 decoded byte array.
     */
    static std::string decode(const std::string &base64Encoded);

    /**
     * Discards any characters outside of the base64 alphabet, per the requirements on page 25 of RFC 2045 - "Any
     * characters outside of the base64 alphabet are to be ignored in base64 encoded data."
     *
     * @param data The base-64 encoded data to groom
     * @return The data, less non-base64 characters (see RFC 2045).
     */
    static std::string discardNonBase64(const std::string &data);
};
}
}
}

#endif //#if !defined(CETTY_SHIRO_CODEC_BASE64_H)
