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

#include <cetty/buffer/BigEndianHeapChannelBuffer.h>

#include <cetty/buffer/HeapChannelBufferFactory.h>
#include <cetty/buffer/SlicedChannelBuffer.h>
#include <cetty/buffer/ByteOrder.h>

#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

ChannelBufferFactory& BigEndianHeapChannelBuffer::factory() const {
    return HeapChannelBufferFactory::getInstance(ByteOrder::BO_BIG_ENDIAN);
}

cetty::buffer::ByteOrder BigEndianHeapChannelBuffer::order() const {
    return ByteOrder::BO_BIG_ENDIAN;
}

int16_t BigEndianHeapChannelBuffer::getShort(int index) const {
    return (int16_t)((arry[index] << 8) | (arry[index+1] & 0xFF));
}

int32_t BigEndianHeapChannelBuffer::getInt(int index) const {
    return ((arry[index]   & 0xff) << 24) |
           ((arry[index+1] & 0xff) << 16) |
           ((arry[index+2] & 0xff) <<  8) |
           ((arry[index+3] & 0xff) <<  0);
}

int64_t BigEndianHeapChannelBuffer::getLong(int index) const {
    return (((int64_t) arry[index]   & 0xff) << 56) |
           (((int64_t) arry[index+1] & 0xff) << 48) |
           (((int64_t) arry[index+2] & 0xff) << 40) |
           (((int64_t) arry[index+3] & 0xff) << 32) |
           (((int64_t) arry[index+4] & 0xff) << 24) |
           (((int64_t) arry[index+5] & 0xff) << 16) |
           (((int64_t) arry[index+6] & 0xff) <<  8) |
           (((int64_t) arry[index+7] & 0xff) <<  0);
}

int BigEndianHeapChannelBuffer::setShort(int index, int value) {
    arry[index  ] = (int8_t)(value >> 8);
    arry[index+1] = (int8_t)(value >> 0);

    return 2;
}

int BigEndianHeapChannelBuffer::setInt(int index, int value) {
    arry[index  ] = (int8_t)(value >> 24);
    arry[index+1] = (int8_t)(value >> 16);
    arry[index+2] = (int8_t)(value >> 8);
    arry[index+3] = (int8_t)(value >> 0);

    return 4;
}

int BigEndianHeapChannelBuffer::setLong(int index, int64_t value) {
    arry[index  ] = (int8_t)(value >> 56);
    arry[index+1] = (int8_t)(value >> 48);
    arry[index+2] = (int8_t)(value >> 40);
    arry[index+3] = (int8_t)(value >> 32);
    arry[index+4] = (int8_t)(value >> 24);
    arry[index+5] = (int8_t)(value >> 16);
    arry[index+6] = (int8_t)(value >>  8);
    arry[index+7] = (int8_t)(value >>  0);

    return 8;
}

cetty::buffer::ChannelBufferPtr BigEndianHeapChannelBuffer::copy(int index, int length) const {
    if (index < 0 || length < 0 || index + length > arry.length()) {
        CETTY_NDC_SCOPE(BigEndianHeapChannelBuffer::copy);
        throw RangeException("out of range.");
    }

    char* copiedBytes = new char[length];
    Array copiedArray(copiedBytes, length);

    Array::copy(copiedArray, 0, this->arry, index, length);
    return ChannelBufferPtr(
               new BigEndianHeapChannelBuffer(copiedArray, true));
}

}
}
