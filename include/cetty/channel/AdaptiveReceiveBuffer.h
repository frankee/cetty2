#if !defined(CETTY_CHANNEL_ADAPTIVERECEIVEBUFFER_H)
#define CETTY_CHANNEL_ADAPTIVERECEIVEBUFFER_H

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

#include <cetty/buffer/ChannelBufferPtr.h>

namespace cetty {
namespace buffer {
class Array;
}
}

namespace cetty {
namespace channel {

using namespace cetty::buffer;

class AdaptiveReceiveBuffer {
public:
    /**
     * Creates a new predictor with the default parameters.  With the default
     * parameters, the expected buffer size starts from <tt>1024</tt>, does not
     * go down below <tt>64</tt>, and does not go up above <tt>65536</tt>.
     */
    AdaptiveReceiveBuffer();

    /**
     * Creates a new predictor with the specified parameters.
     *
     * @param minimum  the inclusive lower bound of the expected buffer size
     * @param initial  the initial buffer size when no feed back was received
     * @param maximum  the inclusive upper bound of the expected buffer size
     */
    AdaptiveReceiveBuffer(int minimum, int initial, int maximum);

    const ChannelBufferPtr& channelBuffer() const {
        return this->buffer;
    }

    void setReceiveBufferSize(int bufferSize);
    void writableBytes(Array* arry);

protected:
    virtual int  nextReceiveBufferSize() const;
    virtual void previousReceiveBufferSize(int receiveBufferSize);

private:
    void init(int minimum, int initial, int maximum);

protected:
    bool decreaseNow;
    int  minIndex;
    int  maxIndex;
    int  index;
    int  nextReceiveBuffSz;

private:
    ChannelBufferPtr buffer;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_ADAPTIVERECEIVEBUFFER_H)

// Local Variables:
// mode: c++
// End:
