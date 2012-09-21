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

#include <cetty/buffer/ChannelBufferUtil.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

static char HEXDUMP_TABLE[256 << 1];
static const char DIGITS[16] =
{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

class HexDumpTableInitializer {
public:
    HexDumpTableInitializer() {
        for (int i = 0; i < 256; ++i) {
            HEXDUMP_TABLE[(i << 1) + 0] = DIGITS[(i >> 4) & 0x0F];
            HEXDUMP_TABLE[(i << 1) + 1] = DIGITS[(i >> 0) & 0x0F];
        }
    }
};

std::string ChannelBufferUtil::hexDump(const ChannelBufferPtr& buffer, int fromIndex, int length) {
    std::string dump;

    if (!buffer) {
        dump = "NULL";
    }
    else {
        hexDump(buffer, fromIndex, length, &dump);
    }

    return dump;
}

std::string ChannelBufferUtil::hexDump(const ChannelBufferPtr& buffer) {
    if (!buffer) {
        return "NULL";
    }

    return hexDump(buffer, buffer->readerIndex(), buffer->readableBytes());
}

void ChannelBufferUtil::hexDump(const ChannelBufferPtr& buffer, std::string* dump) {
    if (NULL == dump || !buffer) {
        return;
    }

    hexDump(buffer, buffer->readerIndex(), buffer->readableBytes(), dump);
}

void ChannelBufferUtil::hexDump(const ChannelBufferPtr& buffer, int fromIndex, int length, std::string* dump) {
    static HexDumpTableInitializer initializer;

    if (NULL == dump || !buffer || length <= 0) {
        return;
    }

    int endIndex = fromIndex + length;
    dump->reserve(dump->size() + length);

    int srcIdx = fromIndex;
    int dstIdx = 0;

    for (; srcIdx < endIndex; ++srcIdx, dstIdx += 2) {
        dump->push_back(HEXDUMP_TABLE[(buffer->getUnsignedByte(srcIdx)) << 1]);
        dump->push_back(HEXDUMP_TABLE[((buffer->getUnsignedByte(srcIdx)) << 1) + 1]);
    }
}

int ChannelBufferUtil::hashCode(const ChannelBufferPtr& buffer) {
    if (!buffer) {
        return 0;
    }

    int aLen = buffer->readableBytes();
    int intCount = aLen >> 2;
    int byteCount = aLen & 3;

    int hashCode = 1;
    int arrayIndex = buffer->readerIndex();

    if (buffer->order() == ByteOrder::BIG) {
        for (int i = intCount; i > 0; --i) {
            hashCode = 31 * hashCode + buffer->getInt(arrayIndex);
            arrayIndex += 4;
        }
    }
    else {
        for (int i = intCount; i > 0; --i) {
            hashCode = 31 * hashCode + swapInt(buffer->getInt(arrayIndex));
            arrayIndex += 4;
        }
    }

    for (int i = byteCount; i > 0; --i) {
        hashCode = 31 * hashCode + buffer->getByte(arrayIndex++);
    }

    if (hashCode == 0) {
        hashCode = 1;
    }

    return hashCode;
}

bool ChannelBufferUtil::equals(const ConstChannelBufferPtr& lbuffer,
                               const ConstChannelBufferPtr& rbuffer) {
    if (!lbuffer || !rbuffer) { return false; }

    if (lbuffer.get() == rbuffer.get()) { return true; }

    int len = lbuffer->readableBytes();

    if (len != rbuffer->readableBytes()) {
        return false;
    }

    int longCount = len >> 3;
    int byteCount = len & 7;

    int aIndex = lbuffer->readerIndex();
    int bIndex = rbuffer->readerIndex();

    if (lbuffer->order() == rbuffer->order()) {
        for (int i = longCount; i > 0; --i) {
            if (lbuffer->getLong(aIndex) != rbuffer->getLong(bIndex)) {
                return false;
            }

            aIndex += 8;
            bIndex += 8;
        }
    }
    else {
        for (int i = longCount; i > 0; --i) {
            if (lbuffer->getLong(aIndex) != swapLong(rbuffer->getLong(bIndex))) {
                return false;
            }

            aIndex += 8;
            bIndex += 8;
        }
    }

    for (int i = byteCount; i > 0; --i) {
        if (lbuffer->getByte(aIndex) != rbuffer->getByte(bIndex)) {
            return false;
        }

        ++aIndex;
        ++bIndex;
    }

    return true;
}

int ChannelBufferUtil::compare(const ConstChannelBufferPtr& lBuffer, const ConstChannelBufferPtr& rBuffer) {
    if (!lBuffer) {
        return !rBuffer ? 0 : 1;
    }

    if (!rBuffer) {
        return -1;
    }

    int aLen = lBuffer->readableBytes();
    int bLen = rBuffer->readableBytes();
    int minLength = std::min(aLen, bLen);

    int uintCount = (minLength >> 2);
    int byteCount = (minLength & 3);

    int aIndex = lBuffer->readerIndex();
    int bIndex = rBuffer->readerIndex();

    if (lBuffer->order() == rBuffer->order()) {
        for (int i = uintCount; i > 0; --i) {
            uint32_t va = lBuffer->getUnsignedInt(aIndex);
            uint32_t vb = rBuffer->getUnsignedInt(bIndex);

            if (va > vb) {
                return 1;
            }
            else if (va < vb) {
                return -1;
            }

            aIndex += 4;
            bIndex += 4;
        }
    }
    else {
        for (int i = uintCount; i > 0; --i) {
            uint32_t va = lBuffer->getUnsignedInt(aIndex);
            uint32_t vb = swapInt(rBuffer->getInt(bIndex)) & 0xFFFFFFFFL;

            if (va > vb) {
                return 1;
            }
            else if (va < vb) {
                return -1;
            }

            aIndex += 4;
            bIndex += 4;
        }
    }

    for (int i = byteCount; i > 0; --i) {
        int8_t va = lBuffer->getByte(aIndex);
        int8_t vb = rBuffer->getByte(bIndex);

        if (va > vb) {
            return 1;
        }
        else if (va < vb) {
            return -1;
        }

        ++aIndex;
        ++bIndex;
    }

    return aLen - bLen;
}

int ChannelBufferUtil::indexOf(const ConstChannelBufferPtr& buffer,
                               int fromIndex,
                               int toIndex,
                               int8_t value) {
    if (fromIndex <= toIndex) {
        return firstIndexOf(buffer, fromIndex, toIndex, value);
    }
    else {
        return lastIndexOf(buffer, fromIndex, toIndex, value);
    }
}

int ChannelBufferUtil::indexOf(const ConstChannelBufferPtr& buffer,
                               int fromIndex,
                               int toIndex,
                               const ChannelBufferIndexFinder::Functor& indexFinder) {
    if (fromIndex <= toIndex) {
        return firstIndexOf(buffer, fromIndex, toIndex, indexFinder);
    }
    else {
        return lastIndexOf(buffer, fromIndex, toIndex, indexFinder);
    }
}

int16_t ChannelBufferUtil::swapShort(int16_t value) {
    return ((value << 8) | ((value >> 8) & 0xff));
}

int ChannelBufferUtil::swapMedium(int value) {
    return ((value << 16) & 0xff0000) | (value & 0xff00) | ((value >> 16) & 0xff);
}

int ChannelBufferUtil::swapInt(int value) {
    return (swapShort((int16_t)value) <<  16) |
           (swapShort((int16_t)(value >> 16)) & 0xffff);
}

int64_t ChannelBufferUtil::swapLong(int64_t value) {
    return ((int64_t)(swapInt((int32_t)value)) << 32) |
           (swapInt((int32_t)(value >> 32)) & 0xffffffffL);
}

int ChannelBufferUtil::firstIndexOf(const ConstChannelBufferPtr& buffer,
                                    int fromIndex,
                                    int toIndex,
                                    const ChannelBufferIndexFinder::Functor& indexFinder) {
    if (!buffer || !indexFinder) {
        return -1;
        // log "the input buffer or the index finder is NULL."
    }

    int capacity = buffer->capacity();
    fromIndex = std::max(fromIndex, 0);
    toIndex = std::min(toIndex, capacity);

    if (fromIndex >= toIndex || capacity == 0) {
        return -1;
    }

    for (int i = fromIndex; i < toIndex; i ++) {
        if (indexFinder(*buffer, i)) {
            return i;
        }
    }

    return -1;
}

int ChannelBufferUtil::firstIndexOf(const ConstChannelBufferPtr& buffer,
                                    int fromIndex,
                                    int toIndex,
                                    int8_t value) {

    if (!buffer) {
        //log "the input buffer is NULL."
        return -1;
    }

    int capacity = buffer->capacity();
    fromIndex = std::max(fromIndex, 0);
    toIndex = std::min(toIndex, capacity);

    if (fromIndex >= toIndex || capacity == 0) {
        return -1;
    }

    for (int i = fromIndex; i < toIndex; ++i) {
        if (buffer->getByte(i) == value) {
            return i;
        }
    }

    return -1;
}

int ChannelBufferUtil::lastIndexOf(const ConstChannelBufferPtr& buffer,
                                   int fromIndex,
                                   int toIndex,
                                   const ChannelBufferIndexFinder::Functor& indexFinder) {
    if (!buffer || !indexFinder) {
        //log "the input buffer or the index finder is NULL."
        return -1;
    }

    int capacity = buffer->capacity();
    fromIndex = std::min(fromIndex, capacity);
    toIndex = std::max(toIndex, 0);

    if (fromIndex < 0 || capacity == 0) {
        return -1;
    }

    for (int i = fromIndex - 1; i >= toIndex; i --) {
        if (indexFinder(*buffer, i)) {
            return i;
        }
    }

    return -1;
}

int ChannelBufferUtil::lastIndexOf(const ConstChannelBufferPtr& buffer,
                                   int fromIndex,
                                   int toIndex,
                                   int8_t value) {
    if (!buffer) {
        // log "the input buffer is NULL."
        return -1;
    }

    int capacity = buffer->capacity();
    fromIndex = std::min(fromIndex, capacity);
    toIndex = std::max(toIndex, 0);

    if (fromIndex < 0 || capacity == 0) {
        return -1;
    }

    for (int i = fromIndex - 1; i >= toIndex; --i) {
        if (buffer->getByte(i) == value) {
            return i;
        }
    }

    return -1;
}

}
}
