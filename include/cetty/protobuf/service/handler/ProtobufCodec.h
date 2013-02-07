#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGECODEC_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGECODEC_H

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

#include <cetty/Types.h>
#include <cetty/buffer/ChannelBuffer.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::buffer;

class ProtobufCodec {
public:
    enum WireType {
        WIRETYPE_VARINT           = 0,
        WIRETYPE_FIXED64          = 1,
        WIRETYPE_LENGTH_DELIMITED = 2,
        WIRETYPE_START_GROUP      = 3,
        WIRETYPE_END_GROUP        = 4,
        WIRETYPE_FIXED32          = 5,
    };

public:
    static int getTagWireType(int64_t tag);
    static int getTagFieldNumber(int64_t tag);

    static bool decodeField(const ChannelBufferPtr& buffer,
                            int* wireType,
                            int* fieldNumber,
                            int* fieldLength);

    static int64_t decodeFixed64(const ChannelBufferPtr& buffer);
    static int decodeFixed32(const ChannelBufferPtr& buffer);
    static int64_t decodeVarint(const ChannelBufferPtr& buffer);

    static void encodeFixed64(const ChannelBufferPtr& buffer, int64_t data);
    static void encodeFixed32(const ChannelBufferPtr& buffer, int data);

    static void encodeTag(const ChannelBufferPtr& buffer,
                          int fieldNum,
                          int type);

    static void encodeVarint(const ChannelBufferPtr& buffer, int64_t val);

private:
    ProtobufCodec() {}
    ~ProtobufCodec() {}

private:
    static const int TAG_TYPE_BITS = 3;
    // Mask for those bits.
    static const int TAG_TYPE_MASK = (1 << TAG_TYPE_BITS) - 1;
};

inline
int ProtobufCodec::getTagWireType(int64_t tag) {
    return static_cast<int>(tag & TAG_TYPE_MASK);
}

inline
int ProtobufCodec::getTagFieldNumber(int64_t tag) {
    return static_cast<int>(tag >> TAG_TYPE_BITS);
}

inline
int64_t ProtobufCodec::decodeFixed64(const ChannelBufferPtr& buffer) {
    return buffer->readLong();
}

inline
int ProtobufCodec::decodeFixed32(const ChannelBufferPtr& buffer) {
    return buffer->readInt();
}

inline
void ProtobufCodec::encodeFixed64(const ChannelBufferPtr& buffer,
                                  int64_t data) {
    buffer->writeLong(data);
}

inline
void ProtobufCodec::encodeFixed32(const ChannelBufferPtr& buffer,
                                  int data) {
    buffer->writeInt(data);
}

inline
void ProtobufCodec::encodeTag(const ChannelBufferPtr& buffer,
                              int fieldNum,
                              int type) {
    int tag = (fieldNum << 3) | type;
    encodeVarint(buffer, tag);
}

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGECODEC_H)

// Local Variables:
// mode: c++
// End:
