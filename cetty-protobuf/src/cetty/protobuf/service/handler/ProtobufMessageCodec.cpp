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

#include <cetty/protobuf/service/handler/ProtobufMessageCodec.h>

#include <cetty/channel/ChannelMessage.h>
#include <cetty/protobuf/service/proto/service.pb.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::protobuf::service::handler;
using namespace cetty::buffer;

bool ProtobufMessageCodec::decodeField(const ChannelBufferPtr& buffer, int* wireType, int* fieldNumber, int* fieldLength) {
    if (NULL == wireType || NULL == fieldNumber || NULL == fieldLength) {
        return false;
    }

    boost::int64_t tag = decodeVarint(buffer);
    *wireType = getTagWireType(tag);
    *fieldNumber = getTagFieldNumber(tag);

    if (*wireType == WIRETYPE_LENGTH_DELIMITED) {
        *fieldLength = (int)decodeVarint(buffer);
    }
    return true;
}

int64_t ProtobufMessageCodec::decodeFixed64(const ChannelBufferPtr& buffer) {
    return buffer->readLong();
}


int ProtobufMessageCodec::decodeFixed32(const ChannelBufferPtr& buffer) {
    return buffer->readInt();
}

boost::int64_t ProtobufMessageCodec::decodeVarint(const ChannelBufferPtr& buffer) {
    uint8_t temp;
    boost::int64_t ret;
    int i = 0;
    while (true) {
        temp = buffer->readByte();

        //如果有后续的字节需要拼接
        if (temp & 0x80) {
            //先左右移一下去掉mab，然后再右移指定的位数，因为varint是低位在前，高位在后
            ret |= (temp << 1 >> 1) << (7 * i);
            ++i;
        }
        else {
            ret |= temp << (7 * i);
            //达到最后一个数据，并且是最高位
            break;
        }
    }
    return ret;
}

void  ProtobufMessageCodec::encodeFixed64(const ChannelBufferPtr& buffer, boost::int64_t data) {
    buffer->writeLong(data);
}

void  ProtobufMessageCodec::encodeFixed32(const ChannelBufferPtr& buffer,int data) {
    buffer->writeInt(data);
}

void ProtobufMessageCodec::encodeTag(const ChannelBufferPtr& buffer,int fieldNum,int type) {
    int tag = (fieldNum << 3) | type;
    encodeVarint(buffer, tag);
}

void ProtobufMessageCodec::encodeVarint(const ChannelBufferPtr& buffer, boost::int64_t val) {
    boost::uint8_t buf[10];
    int varintSize = 0;

    do {
        uint8_t byte = val & 0x7f;
        val >>= 7;

        if (val) { byte |= 0x80; }

        buf[varintSize] = byte;
        varintSize++;
    }
    while (val);

	//write to buffer
	Array array((char*)(buf-varintSize),varintSize);
	buffer->writeBytes(array);
}

}
}
}
}