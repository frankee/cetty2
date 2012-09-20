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

#include <cetty/buffer/HeapChannelBufferFactory.h>

#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

ChannelBufferFactoryPtr HeapChannelBufferFactory::INSTANCE_BE =
    new HeapChannelBufferFactory(ByteOrder::BO_BIG_ENDIAN);
ChannelBufferFactoryPtr HeapChannelBufferFactory::INSTANCE_LE =
    new HeapChannelBufferFactory(ByteOrder::BO_LITTLE_ENDIAN);

ChannelBufferFactory& HeapChannelBufferFactory::getInstance() {
    return *INSTANCE_BE;
}

ChannelBufferFactory& HeapChannelBufferFactory::getInstance(const ByteOrder& endianness) {
    if (endianness == ByteOrder::BO_BIG_ENDIAN) {
        return *INSTANCE_BE;
    }
    else if (endianness == ByteOrder::BO_LITTLE_ENDIAN) {
        return *INSTANCE_LE;
    }
    else {
        CETTY_NDC_SCOPE(HeapChannelBufferFactory::getInstance);
        throw IllegalStateException("Unknown Endian, Should not reach here.");
    }
}

cetty::buffer::ChannelBufferPtr HeapChannelBufferFactory::getBuffer(const ByteOrder& order, int capacity) {
    return ChannelBuffers::buffer(order, capacity);
}

}
}
