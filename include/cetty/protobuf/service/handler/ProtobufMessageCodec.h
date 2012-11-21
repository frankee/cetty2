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
#include <cetty/buffer/ChannelBufferPtr.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::buffer;

class ProtobufMessageCodec {
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
    inline static int getTagWireType(int64_t tag) {
        return static_cast<int>(tag & TAG_TYPE_MASK);
    }

    inline static int getTagFieldNumber(int64_t tag) {
        return static_cast<int>(tag >> TAG_TYPE_BITS);
    }

    static bool decodeField(const ChannelBufferPtr& buffer,
                            int* wireType,
                            int* fieldNumber,
                            int* fieldLength);

    static int64_t decodeFixed64(const ChannelBufferPtr& buffer);
    static int decodeFixed32(const ChannelBufferPtr& buffer);
    static int decodeVarint(const ChannelBufferPtr& buffer);

    static void encodeFixed64(const ChannelBufferPtr& buffer, int64_t data);
    static void encodeFixed32(const ChannelBufferPtr& buffer, int data);

    static void encodeTag(const ChannelBufferPtr& buffer,
                          int fieldNum,
                          int type);

    static void encodeVarint(const ChannelBufferPtr& buffer, int val);

private:
    ProtobufMessageCodec() {}
    ~ProtobufMessageCodec() {}

private:
    static const int TAG_TYPE_BITS = 3;
    // Mask for those bits.
    static const int TAG_TYPE_MASK = (1 << TAG_TYPE_BITS) - 1;
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGECODEC_H)

// Local Variables:
// mode: c++
// End:
