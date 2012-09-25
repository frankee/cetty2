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
#include <cetty/buffer/Unpooled.h>

#include <boost/integer.hpp>

#include <cetty/Types.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/HeapChannelBuffer.h>
#include <cetty/buffer/SlicedChannelBuffer.h>
#include <cetty/buffer/CompositeChannelBuffer.h>

#include <cetty/util/StlUtil.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

ChannelBufferPtr Unpooled::EMPTY_BUFFER =
    ChannelBufferPtr(new HeapChannelBuffer(0, 0, 0));

ChannelBufferPtr Unpooled::buffer(int initialCapacity, int aheadBytes, int maxCapacity) {
    if (initialCapacity == 0) {
        return EMPTY_BUFFER;
    }

    return ChannelBufferPtr(new HeapChannelBuffer(initialCapacity, aheadBytes, maxCapacity));

    return EMPTY_BUFFER;
}

cetty::buffer::ChannelBufferPtr Unpooled::buffer() {
    return buffer(256, 0);
}

cetty::buffer::ChannelBufferPtr Unpooled::buffer(int initialCapacity) {
    return buffer(initialCapacity, 0);
}

ChannelBufferPtr Unpooled::wrappedBuffer(std::string* str) {
    char* bytes = string_as_array(str);
    return wrappedBuffer(bytes, 0, (int)str->size());
}

ChannelBufferPtr Unpooled::wrappedBuffer(char* bytes, int length) {
    if (NULL == bytes || length <= 0) {
        return EMPTY_BUFFER;
    }

    return new HeapChannelBuffer(bytes, length);
}

ChannelBufferPtr Unpooled::wrappedBuffer(char* bytes, int offset, int length) {
    if (length == 0) {
        return EMPTY_BUFFER;
    }

    if (offset == 0) {
        return wrappedBuffer(bytes, length);
    }

    return new SlicedChannelBuffer(wrappedBuffer(bytes, length), offset, length);
}

ChannelBufferPtr Unpooled::wrappedBuffer(const std::vector<ChannelBufferPtr>& buffers) {
    if (buffers.size() == 0) { return EMPTY_BUFFER; }

    std::vector<ChannelBufferPtr> buffs;

    for (size_t i = 0; i < buffers.size(); ++i) {
        if (!buffers[i]->readable()) { continue; }

        if (!buffs.empty() && buffers[i]->order() != buffs[0]->order()) {
            // TODO: release resources.
            throw InvalidArgumentException("inconsistent byte order");
        }

        boost::intrusive_ptr<CompositeChannelBuffer> compositeChannelBuff =
            boost::dynamic_pointer_cast<CompositeChannelBuffer>(buffers[i]);

        if (compositeChannelBuff) {
            // Expand nested composition.
            std::vector<ChannelBufferPtr> vec
                = compositeChannelBuff->decompose();

            buffs.insert(buffs.end(), vec.begin(), vec.end());
        }
        else {
            // An ordinary buffer (non-composite)
            buffs.push_back(buffers[i]->slice());
        }
    }

    if (buffs.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffs.size() == 1) {
        return buffs[0];
    }

    return compositeBuffer(buffs);
}

ChannelBufferPtr
Unpooled::wrappedBuffer(const ChannelBufferPtr& buffer) {
    if (buffer && buffer->readable()) {
        return buffer->slice();
    }
    else {
        return EMPTY_BUFFER;
    }
}

ChannelBufferPtr
Unpooled::wrappedBuffer(const ChannelBufferPtr& buffer0,
                        const ChannelBufferPtr& buffer1) {
    std::vector<ChannelBufferPtr> buffers;

    if (buffer0 && buffer0->readable()) { buffers.push_back(buffer0); }

    if (buffer1 && buffer1->readable()) { buffers.push_back(buffer1); }

    if (buffers.empty()) { return EMPTY_BUFFER; }

    return wrappedBuffer(buffers);
}

ChannelBufferPtr
Unpooled::wrappedBuffer(const ChannelBufferPtr& buffer0,
                        const ChannelBufferPtr& buffer1,
                        const ChannelBufferPtr& buffer2) {
    std::vector<ChannelBufferPtr> buffers;

    if (buffer0 && buffer0->readable()) { buffers.push_back(buffer0); }

    if (buffer1 && buffer1->readable()) { buffers.push_back(buffer1); }

    if (buffer2 && buffer2->readable()) { buffers.push_back(buffer2); }

    if (buffers.empty()) { return EMPTY_BUFFER; }

    return wrappedBuffer(buffers);
}

ChannelBufferPtr
Unpooled::compositeBuffer(const std::vector<ChannelBufferPtr>& components) {
    switch (components.size()) {
    case 0:
        return EMPTY_BUFFER;

    case 1:
        return components[0];

    default:
        return ChannelBufferPtr(
                   new CompositeChannelBuffer(components));
    }
}

ChannelBufferPtr Unpooled::copiedBuffer(const char* bytes, int length) {
    return copiedBuffer(StringPiece(bytes, length));
}

ChannelBufferPtr Unpooled::copiedBuffer(const StringPiece& bytes) {
    if (bytes.empty()) {
        return EMPTY_BUFFER;
    }

    int newCapacity = bytes.length();
    char* newBytes = new char[newCapacity];
    memcpy(newBytes, bytes.data(), newCapacity);

    return new HeapChannelBuffer(newBytes, newCapacity, MAX_INT32, true);
}

ChannelBufferPtr Unpooled::copiedBuffer(const StringPiece& bytes, int offset, int length) {
    if (length <= 0 || bytes.empty() || offset >= bytes.length()) {
        return EMPTY_BUFFER;
    }

    int newCapacity = bytes.length();
    char* newBytes = new char[newCapacity];
    memcpy(newBytes, bytes.data() + offset, newCapacity);

    return new HeapChannelBuffer(newBytes, newCapacity, MAX_INT32, true);
}

ChannelBufferPtr
Unpooled::copiedBuffer(const ChannelBufferPtr& buffer) {
    if (buffer && buffer->readable()) {
        return buffer->copy();
    }
    else {
        return EMPTY_BUFFER;
    }
}

ChannelBufferPtr
Unpooled::copiedBuffer(const ChannelBufferPtr& buffer0,
                       const ChannelBufferPtr& buffer1) {
    if (buffer0 && buffer0->readable()) {
        if (buffer1 && buffer1->readable()) {
            if (buffer1->order() != buffer0->order()) {
                throw InvalidArgumentException("inconsistent byte order");
            }

            return wrappedBuffer(buffer0->copy(), buffer1->copy());
        }
        else {
            return buffer0->copy();
        }
    }
    else {
        if (buffer1 && buffer1->readable()) {
            return buffer1->copy();
        }
        else {
            return EMPTY_BUFFER;
        }
    }
}

ChannelBufferPtr
Unpooled::copiedBuffer(const ChannelBufferPtr& buffer0,
                       const ChannelBufferPtr& buffer1,
                       const ChannelBufferPtr& buffer2) {
    std::vector<ChannelBufferPtr> buffers;

    if (buffer0 && buffer0->readable()) { buffers.push_back(buffer0->copy()); }

    if (buffer1 && buffer1->readable()) {
        if (buffers.size() > 1 && buffer1->order() != buffers[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }

        buffers.push_back(buffer1->copy());
    }

    if (buffer2 && buffer2->readable()) {
        if (buffers.size() > 1 && buffer2->order() != buffers[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }

        buffers.push_back(buffer2->copy());
    }

    if (buffers.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffers.size() == 1) {
        return buffers.front();
    }
    else {
        return wrappedBuffer(buffers);
    }
}

ChannelBufferPtr
Unpooled::copiedBuffer(const std::vector<ChannelBufferPtr>& buffers) {
    if (buffers.size() == 0) { return EMPTY_BUFFER; }

    std::vector<ChannelBufferPtr> buffs;

    for (size_t i = 0; i < buffers.size(); ++i) {
        if (!buffs.empty() && buffers[i]->order() != buffs[0]->order()) {
            throw InvalidArgumentException("inconsistent byte order");
        }

        buffs.push_back(Unpooled::copiedBuffer(buffers[i]));
    }

    if (buffs.empty()) {
        return EMPTY_BUFFER;
    }
    else if (buffs.size() == 1) {
        return buffs[0];
    }

    return wrappedBuffer(buffs);
}

}
}
