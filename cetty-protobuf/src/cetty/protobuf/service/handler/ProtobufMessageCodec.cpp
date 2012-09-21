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

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/protobuf/service/service.pb.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::buffer;
using namespace cetty::protobuf::service::handler;

bool ProtobufMessageCodec::decodeField(const ChannelBufferPtr& buffer, int* wireType, int* fieldNumber, int* fieldLength) {
    if (NULL == wireType || NULL == fieldNumber || NULL == fieldLength) {
        return false;
    }

    int tag = decodeVarint(buffer);
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

int ProtobufMessageCodec::decodeVarint(const ChannelBufferPtr& buffer) {
    int temp = 0;
    int ret = 0;
    int i = 0;
	int off = 0;
    while (true) {
        temp = buffer->readByte();

        //如果有后续的字节需要拼接
        if (temp & 0x80) {
            //先左右移一下去掉mab，然后再右移指定的位数，因为varint是低位在前，高位在后
            ret |= (temp << 1 >> 1) << (7 * i);
            ++i;
        }
        else {
			off = 7*i;
			ret = ret + (temp<<off);
            //ret = ret + (temp<<0);
            //达到最后一个数据，并且是最高位
            break;
        }
    }
    return ret;
}

void  ProtobufMessageCodec::encodeFixed64(const ChannelBufferPtr& buffer, int64_t data) {
    buffer->writeLong(data);
}

void  ProtobufMessageCodec::encodeFixed32(const ChannelBufferPtr& buffer,int data) {
    buffer->writeInt(data);
}

void ProtobufMessageCodec::encodeTag(const ChannelBufferPtr& buffer,int fieldNum,int type) {
    int tag = (fieldNum << 3) | type;
    ProtobufMessageCodec::encodeVarint(buffer, tag);
}

void ProtobufMessageCodec::encodeVarint(const ChannelBufferPtr& buffer, int val) {
    uint8_t buf[10];
    int varintSize = 0;
    do {
        uint8_t byte = val & 0x7f;

		if (val>>7) 
		{
			byte |= 0x80;
		}

        buf[varintSize] = byte;
		varintSize++;
	    val >>= 7;
    }
    while (val);

	//write to buffer
	StringPiece bytes((const char*)buf, varintSize);
	buffer->writeBytes(bytes);
}

}
}
}
}
