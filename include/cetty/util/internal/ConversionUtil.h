#if !defined(CETTY_UTIL_INTERNAL_CONVERSIONUTIL_H)
#define CETTY_UTIL_INTERNAL_CONVERSIONUTIL_H

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

#include <string>
#include <boost/any.hpp>

namespace cetty {
namespace util {
namespace internal {

/**
 * Conversion utility class to parse a property represented as a string or
 * an object.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */
class ConversionUtil {
public:
    /**
     * Converts the specified object into an integer.
     */
    static int toInt(const boost::any& value) {
        const int* v = boost::any_cast<int>(&value);

        if (v) {
            return *v;
        }
        else { // for parse string of int
            return 0;
        }
    }

    /**
     * Converts the specified object into a bool.
     */
    static bool toBoolean(const boost::any& value) {
        const bool* bv = boost::any_cast<bool>(&value);

        if (bv) {
            return *bv;
        }

        const int* iv = boost::any_cast<int>(&value);

        if (iv) {
            return *iv != 0;
        }

        const char* const* cstr = boost::any_cast<const char*>(&value);

        if (cstr) {
            if (NULL == *cstr || strlen(*cstr) == 0) {
                return false;
            }

            char first = (*cstr)[0];

            if ('Y' == first || 'T' == first || 'y' == first || 't' == first) {
                return true;
            }

            if (atoi(*cstr)) { return true; }

            return false;
        }

        const std::string* str = boost::any_cast<std::string>(&value);

        if (str) {
            if (str->length() == 0) {
                return false;
            }

            char first = str->at(0);

            if ('Y' == first || 'T' == first || 'y' == first || 't' == first) {
                return true;
            }

            if (atoi(str->c_str())) { return true; }

            return false;
        }

        return false;
    }

    /**
     * Converts the specified object into an array of strings.
     */
    //     static String[] toStringArray(Object value) {
    //         if (value instanceof String[]) {
    //             return (String[]) value;
    //         }
    //
    //         if (value instanceof Iterable<?>) {
    //             List<String> answer = new ArrayList<String>();
    //             for (Object v: (Iterable<?>) value) {
    //                 if (v == null) {
    //                     answer.add(null);
    //                 } else {
    //                     answer.add(String.valueOf(v));
    //                 }
    //             }
    //             return answer.toArray(new String[answer.size()]);
    //         }
    //
    //         return String.valueOf(value).split("[, \\t\\n\\r\\f\\e\\a]");
    //     }

    static std::string toString(bool value) {
        if (value) {
            return "TRUE";
        }
        else {
            return "FALSE";
        }
    }

    static std::string toString(int value);

private:
    ConversionUtil() {}
};

}
}
}

#endif //#if !defined(CETTY_UTIL_INTERNAL_CONVERSIONUTIL_H)

// Local Variables:
// mode: c++
// End:

