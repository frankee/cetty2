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

#include <cetty/protobuf/service/handler/ProtobufServiceMessageCodec.h>

#include <cetty/channel/ChannelMessage.h>
#include <cetty/protobuf/service/proto/service.pb.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::protobuf::service::handler;
using namespace cetty::buffer;

bool ProtobufServiceMessageCodec::decodeField(const ChannelBufferPtr& buffer, int* wireType, int* fieldNumber, int* fieldLength) {
    if (NULL == wireType || NULL == fieldNumber || NULL == fieldLength) {
        return false;
    }

    int64_t tag = decodeVarint(buffer);
    *wireType = getTagWireType(tag);
    *fieldNumber = getTagFieldNumber(tag);

    if (*wireType == WIRETYPE_LENGTH_DELIMITED) {
        *fieldLength = (int)decodeVarint(buffer);
    }

    return true;
}

int64_t ProtobufServiceMessageCodec::decodeFixed64(ChannelBufferPtr& buffer) {
    return buffer->readLong();
}


int ProtobufServiceMessageCodec::decodeFixed32(ChannelBufferPtr& buffer) {
    return buffer->readInt();
}

int64_t ProtobufServiceMessageCodec::decodeVarint(ChannelBufferPtr& buffer) {
    uint8_t temp;
    uint64_t ret;
    int i = 0;
    while (true) {
        temp = buffer->readByte();

        //����к������ֽ���Ҫƴ��
        if (temp & 0x80) {
            //��������һ��ȥ��mab��Ȼ��������ָ����λ������Ϊvarint�ǵ�λ��ǰ����λ�ں�
            ret |= (temp << 1 >> 1) << (7 * i);
            ++i;
        }
        else {
            ret |= temp << (7 * i);
            //�ﵽ���һ�����ݣ����������λ
            break;
        }
    }

    return ret;
}

uint64_t  ProtobufServiceMessageCodec::encodeFixed64(ChannelBufferPtr& buffer,uint64_t data) {
    buffer->writeLong(data);
}

uint64_t  ProtobufServiceMessageCodec::encodeFixed32(ChannelBufferPtr& buffer,int data) {
    buffer->writeInt(data);
}

//the flag is indict that if varint is embedded��1:embedded  0:unembedded
//ֻ��Ƕ�׵�varint�������Ҫд tag
uint64_t  ProtobufServiceMessageCodec::varintEncode(ChannelBufferPtr& buffer,uint64_t val,bool  flag) {
    //�����Ƕ�׵ģ��Ȱ��ֽ���д��ǰ�棬�ֽ���Ҳ��varint
    if (flag) {
        varintEncode(buffer,varintSize,0);
        //����Ƕ�׵�ͷ
        encodeTag(buffer,1,0);
    }

    Array array((char*)(buf-varintSize),varintSize);
    buffer->writeBytes(array);
}

void ProtobufServiceMessageCodec::encodeTag(ChannelBufferPtr& buffer,uint64_t fieldNum,uint64_t type) {
    uint64_t tag = (fieldNum<<3)|type;
    varintEncode(buffer, tag);
}

void ProtobufServiceMessageCodec::encodeVarint(ChannelBufferPtr& buffer,uint64_t len) {
    uint8_t* buf;
    uint64_t varintSize = 0;

    do {
        uint8_t byte = val & 0x7f;
        val >>= 7;

        if (val) { byte |= 0x80; }

        *buf++ = byte;
        varintSize++;
    }
    while (val);
}

}
}
}
}