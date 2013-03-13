#if !defined(CETTY_MEMCACHED_MEMCACHEDCLIENT_H)
#define CETTY_MEMCACHED_MEMCACHEDCLIENT_H

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

#include <boost/function.hpp>

namespace cetty {
namespace memcached {

class MemcachedClient : private boost::noncopyable {
public:
    typedef boost::function<void (int, const std::string&)> StorageReplyCallback;

    enum ProtocolType {
        PROTOCOL_TEXT,
        PROTOCOL_BINARY
    };

public:
    MemcachedClient(const ChannelPtr& channel);
    MemcachedClient(const ChannelPtr& channel, ProtocolType type);

    /**
     * Store a key. Override an existing value.
     */

    /**
     * Store a key. Override an existing values.
     */
    void set(const std::string& key, const StringPiece& value) {
        set(key, 0, Time.epoch, value);
    }

    void set(const std::string& key,
             const StringPiece& value,
             int flags,
             int64_t expiry);

    /**
     * Store a key but only if it doesn't already exist on the server.
     * @return true if stored, false if not stored
     */
    void add(const std::string& key, const StringPiece& value) {
        add(key, 0, Time.epoch, value);
    }

    /**
     * Store a key but only if it doesn't already exist on the server.
     * @return true if stored, false if not stored
     */
    void add(const std::string& key,
             int flags,
             int64_t expiry,
             const StringPiece& value);

    /**
     * Append a set of bytes to the end of an existing key. If the key doesn't
     * exist, the operation has no effect.
     * @return true if stored, false if not stored
     */
    void append(const std::string& key, const StringPiece& value) {
        append(key, 0, Time.epoch, value);
    }

    /**
     * Prepend a set of bytes to the beginning of an existing key. If the key
     * doesn't exist, the operation has no effect.
     * @return true if stored, false if not stored
     */
    void prepend(const std::string& key, const StringPiece& value) {
        prepend(key, 0, Time.epoch, value);
    }

    /**
     * Append bytes to the end of an existing key. If the key doesn't exist, the
     * operation has no effect.
     * @return true if stored, false if not stored
     */
    void append(const std::string& key,
                int flags,
                int64_t expiry,
                const StringPiece& value);

    /**
     * Prepend bytes to the beginning of an existing key. If the key doesn't
     * exist, the operation has no effect.
     * @return true if stored, false if not stored
     */
    void prepend(const std::string& key,
                 int flags,
                 int64_t expiry,
                 const StringPiece& value);

    /**
     * Replace an item if it exists. If it doesn't exist, the operation has no
     * effect.
     * @return true if stored, false if not stored
     */
    void replace(const std::string& key, const StringPiece& value) {
        replace(key, 0, Time.epoch, value);
    }

    /**
     * Replace bytes on an existing key. If the key doesn't exist, the
     * operation has no effect.
     * @return true if stored, false if not stored
     */
    void replace(const std::string& key,
                 int flags,
                 int64_t expiry,
                 const StringPiece& value);

    /**
     * Perform a CAS operation on the key, only if the value has not
     * changed since the value was last retrieved, and `casUnique`
     * extracted from a `gets` command.  We treat the "cas unique" token
     * opaquely, but in reality it is a string-encoded u64.
     *
     * @return true if replaced, false if not
     */
    void cas(const std::string& key, const StringPiece& value, const StringPiece& casUnique) {
        cas(key, 0, Time.epoch, value, casUnique)
    }

    /**
     * Perform a CAS operation on the key, only if the value has not
     * changed since the value was last retrieved, and `casUnique`
     * extracted from a `gets` command.  We treat the "cas unique" token
     * opaquely, but in reality it is a string-encoded u64.
     *
     * @return true if replaced, false if not
     */
    void cas(const std::string& key,
             int flags,
             int64_t expiry,
             const StringPiece& value,
             const StringPiece& casUnique);

    /**
     * Get a key from the server.
     */
    void get(const std::string& key);

    /**
     * Get a key from the server, with a "cas unique" token.  The token
     * is treated opaquely by the memcache client but is in reality a
     * string-encoded u64.
     */
    void gets(const std::string& key);

    /**
     * Get a set of keys from the server.
     * @return a Map[String, T] of all of the keys that the server had.
     */
    //     void get(keys: Iterable[String]): Future[Map[String, T]] = {
    //         getResult(keys) flatMap {
    //             result =>
    //
    //             if (result.failures.nonEmpty) {
    //                 Future.exception(result.failures.values.head)
    //             }
    //             else {
    //                 Future.value(result.values mapValues { channelBufferToType(_) })
    //             }
    //         }
    //     }

    /**
     * Get a set of keys from the server, together with a "cas unique"
     * token.  The token is treated opaquely by the memcache client but
     * is in reality a string-encoded u64.
     *
     * @return a Map[String, (T, ChannelBuffer)] of all the
     * keys the server had, together with their "cas unique" token
     */
    // void gets(keys: Iterable[String]): Future[Map[String, (T, ChannelBuffer)]] = {
    //         getsResult(keys) flatMap {
    //             result =>
    //
    //             if (result.failures.nonEmpty) {
    //                 Future.exception(result.failures.values.head)
    //             }
    //             else {
    //                 Future.value(result.valuesWithTokens mapValues {
    //                 case (v, u) => (channelBufferToType(v), u)
    //                     })
    //             }
    //         }
    //     }

    /**
     * Get a set of keys from the server. Returns a Future[GetResult] that
     * encapsulates hits, misses and failures.
     */
    template<typename Iterator>
    void getResult(Iterator begin, Iterator end);

    /**
     * Get a set of keys from the server. Returns a Future[GetsResult] that
     * encapsulates hits, misses and failures. This variant includes the casToken
     * from memcached.
     */
    //void getsResult(keys: Iterable[String]);

    /**
     * Remove a key.
     * @return true if deleted, false if not found
     */
    void del(const std::string& key);

    /**
     * Increment a key. Interpret the value as an Long if it is parsable.
     * This operation has no effect if there is no value there already.
     */
    void increment(const std::string& key)
    void increment(const std::string& key, int64_t delta);

    /**
     * Decrement a key. Interpret the value as an JLong if it is parsable.
     * This operation has no effect if there is no value there already.
     */
    void decrement(const std::string& key);
    void decrement(const std::string& key, int64_t delta);

    /**
     * Send a quit command to the server. Alternative to release, for
     * protocol compatibility.
     * @return none
     */
    void quit();

    /**
     * Send a stats command with optional arguments to the server
     * @return a sequence of strings, each of which is a line of output
     */
    void stats();

};

}
}

#endif //#if !defined(CETTY_MEMCACHED_MEMCACHEDCLIENT_H)

// Local Variables:
// mode: c++
// End:
