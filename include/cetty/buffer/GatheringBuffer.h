#if !defined(CETTY_BUFFER_GATHERINGBUFFER_H)
#define CETTY_BUFFER_GATHERINGBUFFER_H

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

#include <vector>
#include <cetty/util/StringPiece.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

class GatheringBuffer {
public:
    GatheringBuffer() : bytesCnt(0) {}
    ~GatheringBuffer() {}

    bool empty() const {
        return buffers.empty();
    }

    /**
     * underline memory block count.
     */
    int blockCount() const {
        return (int)buffers.size();
    }

    /**
     * the total bytes of all the memory blocks.
     */
    int bytesCount() const {
        return bytesCnt;
    }

    const StringPiece& at(int index) {
        return buffers.at(index);
    }

    void append(const char* data, int size) {
        if (data && size > 0) {
            bytesCnt += size;
            buffers.push_back(StringPiece(data, size));
        }
    }

    void addBlock(const StringPiece& bytes) {
        if (!bytes.empty()) {
            bytesCnt += bytes.size();
            buffers.push_back(bytes);
        }
    }

    void clear() {
        buffers.clear();
    }

private:
    int bytesCnt;
    std::vector<StringPiece> buffers;
};

}
}

#endif //#if !defined(CETTY_BUFFER_GATHERINGBUFFER_H)

// Local Variables:
// mode: c++
// End:
