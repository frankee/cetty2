#if !defined(CETTY_UTIL_CHARSETUTIL_H)
#define CETTY_UTIL_CHARSETUTIL_H

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
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/util/Charset.h>

namespace cetty {
namespace util {

/**
 * A utility class that provides various common operations and constants
 * related with {@link Charset} and its relevant classes.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class CharsetUtil {
public:
    /**
     * 16-bit UTF (UCS Transformation Format) whose byte order is identified by
     * an optional byte-order mark
     */
    static Charset UTF_16;

    /**
     * 16-bit UTF (UCS Transformation Format) whose byte order is big-endian
     */
    static Charset UTF_16BE;

    /**
     * 16-bit UTF (UCS Transformation Format) whose byte order is little-endian
     */
    static Charset UTF_16LE;

    /**
     * 8-bit UTF (UCS Transformation Format)
     */
    static Charset UTF_8;

    /**
     * ISO Latin Alphabet No. 1, as known as <tt>ISO-LATIN-1</tt>
     */
    static Charset ISO_8859_1;

    /**
     * 7-bit ASCII, as known as ISO646-US or the Basic Latin block of the
     * Unicode character set
     */
    static Charset US_ASCII;


#if 0
    /**
     * Returns a cached thread-local {@link CharsetEncoder} for the specified
     * <tt>charset</tt>.
     */
    static CharsetEncoder getEncoder(Charset charset) {
        if (charset == null) {
            throw new NullPointerException("charset");
        }

        Map<Charset, CharsetEncoder> map = encoders.get();
        CharsetEncoder e = map.get(charset);

        if (e != null) {
            e.reset();
            e.onMalformedInput(CodingErrorAction.REPLACE);
            e.onUnmappableCharacter(CodingErrorAction.REPLACE);
            return e;
        }

        e = charset.newEncoder();
        e.onMalformedInput(CodingErrorAction.REPLACE);
        e.onUnmappableCharacter(CodingErrorAction.REPLACE);
        map.put(charset, e);
        return e;
    }

    /**
     * Returns a cached thread-local {@link CharsetDecoder} for the specified
     * <tt>charset</tt>.
     */
    static CharsetDecoder getDecoder(Charset charset) {
        if (charset == null) {
            throw new NullPointerException("charset");
        }

        Map<Charset, CharsetDecoder> map = decoders.get();
        CharsetDecoder d = map.get(charset);

        if (d != null) {
            d.reset();
            d.onMalformedInput(CodingErrorAction.REPLACE);
            d.onUnmappableCharacter(CodingErrorAction.REPLACE);
            return d;
        }

        d = charset.newDecoder();
        d.onMalformedInput(CodingErrorAction.REPLACE);
        d.onUnmappableCharacter(CodingErrorAction.REPLACE);
        map.put(charset, d);
        return d;
    }
#endif

private:
    //     static final ThreadLocal<Map<Charset, CharsetEncoder>> encoders =
    //         new ThreadLocal<Map<Charset,CharsetEncoder>>() {
    //             @Override
    //                 protected Map<Charset, CharsetEncoder> initialValue() {
    //                     return new IdentityHashMap<Charset, CharsetEncoder>();
    //             }
    //     };
    //
    //     static final ThreadLocal<Map<Charset, CharsetDecoder>> decoders =
    //         new ThreadLocal<Map<Charset,CharsetDecoder>>() {
    //             @Override
    //                 protected Map<Charset, CharsetDecoder> initialValue() {
    //                     return new IdentityHashMap<Charset, CharsetDecoder>();
    //             }
    //     };

private:
    CharsetUtil() {}
};

}
}

#endif //#if !defined(CETTY_UTIL_CHARSETUTIL_H)

// Local Variables:
// mode: c++
// End:

