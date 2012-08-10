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
#include <cetty/shiro/codec/Hex.h>
#include <cetty/shiro/codec/Base64.h>

namespace cetty {
namespace shiro {
namespace util {

using namespace cetty::shiro::codec;
/**
 * Very simple {@link ByteSource ByteSource} implementation that maintains an internal {@code byte[]} array and uses the
 * {@link Hex Hex} and {@link Base64 Base64} codec classes to support the
 * {@link #toHex() toHex()} and {@link #toBase64() toBase64()} implementations.
 * <p/>
 * The constructors on this class accept the following implicit byte-backed data types and will convert them to
 * a byte-array automatically:
 * <ul>
 * <li>byte[]</li>
 * <li>char[]</li>
 * <li>String</li>
 * <li>{@link ByteSource ByteSource}</li>
 * <li>{@link File File}</li>
 * <li>{@link InputStream InputStream}</li>
 * </ul>
 *
 * @since 1.0
 */
class ByteSource {

public:
    ByteSource(){}
    ByteSource(const std::string &bytes) {
        this->bytes = bytes;
    }

    /**
     * Creates an instance using the sources bytes directly - it does not create a copy of the
     * argument's byte array.
     *
     * @param source the source to use to populate the underlying byte array.
     * @since 1.1
     */
    ByteSource(const ByteSource &src) {
        this->bytes = src.bytes;
        this->cachedHex = src.cachedHex;
        this->cachedBase64 = src.cachedBase64;
    }

    const std::string& getBytes() {
        return this->bytes;
    }

    const std::string& toHex() const {
        if ( this->cachedHex == "" ) {
            this->cachedHex = Hex::encode(getBytes());
        }
        return this->cachedHex;
    }

    const std::string& toBase64() const {
        if ( this->cachedBase64 == "" ) {
            this->cachedBase64 = Base64::encodeToString(getBytes());
        }
        return this->cachedBase64;
    }

    std::string toString() {
        return toBase64();
    }

    int hashCode();

    bool equals(const ByteSource &obj);

private:
    std::string bytes;

    mutable std::string cachedHex;
    mutable std::string cachedBase64;
};
}
}
}
