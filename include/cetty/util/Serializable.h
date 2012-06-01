#if !defined(CETTY_UTIL_PERSISTABLE_H)
#define CETTY_UTIL_PERSISTABLE_H

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

namespace cetty {
namespace util {

class OutputStream;
class InputStream;

/**
 * Serializable is a simple interface used to indicate that an object
 * can read or write itself to an input or output stream.
 *
 * @see OutputStream
 * @see InputStream
 */

class Serializable {
public:
    virtual ~Serializable() {};

    /**
     * Write the persistable object to the specified output stream
     * @param OutputStream
     */
    virtual void toStream(OutputStream& stream) = 0;

    /**
     * Read the persistable object from the specified input stream
     * @param InputStream
     */
    virtual void fromStream(InputStream& stream) = 0;
};

}
}

#endif //#if !defined(CETTY_UTIL_PERSISTABLE_H)

// Local Variables:
// mode: c++
// End:

