#if !defined(CETTY_BUFFER_SLICEDCHANNELBUFFERTEST_H)
#define CETTY_BUFFER_SLICEDCHANNELBUFFERTEST_H

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

#include "cetty/buffer/AbstractChannelBufferTest.h"

namespace cetty { namespace buffer {


class SlicedChannelBufferTest : public AbstractChannelBufferTest {
public:
    SlicedChannelBufferTest() {}
    virtual ~SlicedChannelBufferTest() {}

protected:
    virtual void newBuffer(int length) {
        char* buf = new char[length * 2];
        memset(buf, 0, (length * 2) * sizeof(char));
        buffer = ChannelBuffers::wrappedBuffer(
                Array(buf, length*2), random.nextInt(length - 1) + 1, length);

        //ASSERT_EQ(length, buffer->writerIndex());
        buffers.push_back(buffer);
    }

    virtual std::vector<ChannelBufferPtr>& components() {
        return buffers;
    }

private:
    cetty::util::Random random;
    std::vector<ChannelBufferPtr> buffers;
};

}}

#endif //#if !defined(CETTY_BUFFER_SLICEDCHANNELBUFFERTEST_H)
