#if !defined(CETTY_UTIL_INPUTSTREAM_H)
#define CETTY_UTIL_INPUTSTREAM_H

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

class Serializable;

/**
 * \class InputStream, basic abstract class, describing the requirements to
 *        support streaming bytes into a stream.
 */
class InputStream {
public:
    /**
    *seek types
    */
    enum SeekType {
        stSeekFromStart=0,
        stSeekForwards,
        stSeekBackwards,
        stSeekFromEnd
    };

    virtual ~InputStream() {};

    virtual void read(boost::int8_t& val);
    virtual boost::int8_t readByte();

    virtual int read(boost::int8_t* bytes, int length);
    virtual int read(boost::int8_t* bytes, int offset, int length);

    virtual int skip(int n);

    virtual int available() const {
        return 0;
    }

    virtual int  mark(int readlimit);
    virtual void reset();
    virtual bool markSupported() const;

    virtual void close() = 0;
};

}
}

#endif //#if !defined(CETTY_UTIL_INPUTSTREAM_H)

// Local Variables:
// mode: c++
// End:
