#if !defined(CETTY_SHIRO_AUTHC_SHA256HASH_H)
#define CETTY_SHIRO_AUTHC_SHA256HASH_H
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
namespace authc {
using namespace cetty::shiro::codec;
/**
 * A {@code Hash} implementation that allows any {@link java.security.MessageDigest MessageDigest} algorithm name to
 * be used.  This class is a less type-safe variant than the other {@code AbstractHash} subclasses
 * (e.g. {@link Sha512Hash}, etc), but it does allow for any algorithm name to be specified in case the other subclass
 * implementations do not represent an algorithm that you may want to use.
 * <p/>
 * As of Shiro 1.1, this class effectively replaces the (now-deprecated) {@link AbstractHash} class.  It subclasses
 * {@code AbstractHash} only to retain backwards-compatibility.
 *
 * @since 1.1
 */
class Sha256Hash{
public:
    static const int SHA_DIGEST_LEN;

public:
    Sha256Hash(){}
    /**
     * Creates an {@code algorithmName}-specific hash of the specified {@code source} with no {@code salt} using a
     * single hash iteration.
     * <p/>
     * This is a convenience constructor that merely executes <code>this( algorithmName, source, null, 1);</code>.
     * <p/>
     * Please see the
     * {@link #SimpleHash(String algorithmName, Object source, Object salt, int numIterations) SimpleHashHash(algorithmName, Object,Object,int)}
     * constructor for the types of Objects that may be passed into this constructor, as well as how to support further
     * types.
     *
     * @param algorithmName the {@link java.security.MessageDigest MessageDigest} algorithm name to use when
     *                      performing the hash.
     * @param source        the object to be hashed.
     * @throws org.apache.shiro.codec.CodecException
     *          if the specified {@code source} cannot be converted into a byte array (byte[]).
     */
    Sha256Hash(const std::string &source){
        init( source, std::string(), 1);
    }

    /**
     * Creates an {@code algorithmName}-specific hash of the specified {@code source} using the given {@code salt}
     * using a single hash iteration.
     * <p/>
     * It is a convenience constructor that merely executes <code>this( algorithmName, source, salt, 1);</code>.
     * <p/>
     * Please see the
     * {@link #SimpleHash(String algorithmName, Object source, Object salt, int numIterations) SimpleHashHash(algorithmName, Object,Object,int)}
     * constructor for the types of Objects that may be passed into this constructor, as well as how to support further
     * types.
     *
     * @param algorithmName the {@link java.security.MessageDigest MessageDigest} algorithm name to use when
     *                      performing the hash.
     * @param source        the source object to be hashed.
     * @param salt          the salt to use for the hash
     * @throws CodecException if either constructor argument cannot be converted into a byte array.
     */
    Sha256Hash(const std::string &source, const std::string &salt)  {
        init(source, salt, 1);
    }

    /**
     * Creates an {@code algorithmName}-specific hash of the specified {@code source} using the given
     * {@code salt} a total of {@code hashIterations} times.
     * <p/>
     * By default, this class only supports Object method arguments of
     * type {@code byte[]}, {@code char[]}, {@link String}, {@link java.io.File File},
     * {@link java.io.InputStream InputStream} or {@link org.apache.shiro.util.ByteSource ByteSource}.  If either
     * argument is anything other than these types a {@link org.apache.shiro.codec.CodecException CodecException}
     * will be thrown.
     * <p/>
     * If you want to be able to hash other object types, or use other salt types, you need to override the
     * {@link #toBytes(Object) toBytes(Object)} method to support those specific types.  Your other option is to
     * convert your arguments to one of the default supported types first before passing them in to this
     * constructor}.
     *
     * @param algorithmName  the {@link java.security.MessageDigest MessageDigest} algorithm name to use when
     *                       performing the hash.
     * @param source         the source object to be hashed.
     * @param salt           the salt to use for the hash
     * @param hashIterations the number of times the {@code source} argument hashed for attack resiliency.
     * @throws CodecException if either Object constructor argument cannot be converted into a byte array.
     */
    Sha256Hash(const std::string &source, const std::string &salt, int hashIterations)  {
        init(source, salt, hashIterations);
    }

    void init(const std::string &source, const std::string &salt, int hashIterations){
        hash(source, salt, hashIterations);
    }

    const std::string &getBytes() const{ return this->bytes; }

    /**
     * Sets the raw bytes stored by this hash instance.
     * <p/>
     * The bytes are kept in raw form - they will not be hashed/changed.  This is primarily a utility method for
     * constructing a Hash instance when the hashed value is already known.
     *
     * @param alreadyHashedBytes the raw already-hashed bytes to store in this instance.
     */
    void setBytes(const std::string &alreadyHashedBytes) {
        this->bytes = alreadyHashedBytes;
        this->hexEncoded = std::string();
        this->base64Encoded = std::string();
    }

    /**
     * Returns a hex-encoded string of the underlying {@link #getBytes byte array}.
     * <p/>
     * This implementation caches the resulting hex string so multiple calls to this method remain efficient.
     * However, calling {@link #setBytes setBytes} will null the cached value, forcing it to be recalculated the
     * next time this method is called.
     *
     * @return a hex-encoded string of the underlying {@link #getBytes byte array}.
     */
    std::string toHex() {
        if (this->hexEncoded.empty()) {
            this->hexEncoded = Hex::encode(getBytes());
        }
        return this->hexEncoded;
    }

    /**
     * Returns a Base64-encoded string of the underlying {@link #getBytes byte array}.
     * <p/>
     * This implementation caches the resulting Base64 string so multiple calls to this method remain efficient.
     * However, calling {@link #setBytes setBytes} will null the cached value, forcing it to be recalculated the
     * next time this method is called.
     *
     * @return a Base64-encoded string of the underlying {@link #getBytes byte array}.
     */
    std::string toBase64() {
        if (this->base64Encoded.empty()) {
            this->base64Encoded = Base64::encode(getBytes());
        }
        return this->base64Encoded;
    }

    /**
     * Returns {@code true} if the specified object is a Hash and its {@link #getBytes byte array} is identical to
     * this Hash's byte array, {@code false} otherwise.
     *
     * @param o the object (Hash) to check for equality.
     * @return {@code true} if the specified object is a Hash and its {@link #getBytes byte array} is identical to
     *         this Hash's byte array, {@code false} otherwise.
     */
    bool equals(const Sha256Hash &o) { return getBytes() == o.getBytes(); }

    static Sha256Hash fromHexString(const std::string &hex) {
        Sha256Hash hash;
        hash.setBytes(Hex::decode(hex));
        return hash;
    }

    static Sha256Hash fromBase64String(const std::string &base64) {
        Sha256Hash hash;
        hash.setBytes(Base64::decode(base64));
        return hash;
    }

protected:
    /**
     * Hashes the specified byte array without a salt for a single iteration.
     *
     * @param bytes the bytes to hash.
     * @return the hashed bytes.
     */
    std::string hash(const std::string &bytes) {
        return hash(bytes, std::string(), 1);
    }

    /**
     * Hashes the specified byte array using the given {@code salt} for a single iteration.
     *
     * @param bytes the bytes to hash
     * @param salt  the salt to use for the initial hash
     * @return the hashed bytes
     */
    std::string hash(const std::string &bytes, const std::string &salt) {
        return hash(bytes, salt, 1);
    }

    /**
     * Hashes the specified byte array using the given {@code salt} for the specified number of iterations.
     *
     * @param bytes          the bytes to hash
     * @param salt           the salt to use for the initial hash
     * @param hashIterations the number of times the the {@code bytes} will be hashed (for attack resiliency).
     * @return the hashed bytes.
     */
    std::string hash(const std::string &bytes, const std::string &salt, int hashIterations);

private:
    /**
     * The hashed data
     */
    std::string bytes;

    /**
     * Cached value of the {@link #toHex() toHex()} call so multiple calls won't incur repeated overhead.
     */
    std::string hexEncoded;

    /**
     * Cached value of the {@link #toBase64() toBase64()} call so multiple calls won't incur repeated overhead.
     */
    std::string base64Encoded;
};
}
}
}
#endif // #if !defined(CETTY_SHIRO_CRYPTO_SHA256HASH_H)
