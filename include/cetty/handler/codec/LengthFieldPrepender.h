#if !defined(CETTY_HANDLER_CODEC_LENGTHFIELDPREPENDER_H)
#define CETTY_HANDLER_CODEC_LENGTHFIELDPREPENDER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <boost/function.hpp>
#include <cetty/util/Exception.h>
#include <cetty/util/Integer.h>
#include <cetty/handler/codec/MessageToBufferEncoder.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::channel;
using namespace cetty::util;

/**
 * An encoder that prepends the length of the message.  The length value is
 * prepended as a binary form.  It is encoded in either big endian or little
 * endian depending on the default {@link ByteOrder} of the current
 * {@link ChannelBufferFactory}.
 * <p>
 * For example, <tt>{@link LengthFieldPrepender}(2)</tt> will encode the
 * following 12-bytes string:
 * <pre>
 * +----------------+
 * | "HELLO, WORLD" |
 * +----------------+
 * </pre>
 * into the following:
 * <pre>
 * +--------+----------------+
 * + 0x000C | "HELLO, WORLD" |
 * +--------+----------------+
 * </pre>
 * If you turned on the <tt>lengthIncludesLengthFieldLength</tt> flag in the
 * constructor, the encoded data would look like the following
 * (12 (original data) + 2 (prepended data) = 14 (0xE)):
 * <pre>
 * +--------+----------------+
 * + 0x000E | "HELLO, WORLD" |
 * +--------+----------------+
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class LengthFieldPrepender : public MessageToBufferEncoder<ChannelBufferPtr> {
public:
    typedef boost::function2<uint32_t, const uint8_t*, int> ChecksumFunction;

public:
    /**
     * Creates a new instance.
     *
     * @param lengthFieldLength the length of the perpended length field.
     *                          Only 1, 2, 4, and 8 are allowed.
     *
     * @throws InvalidArgumentException
     *         if <tt>lengthFieldLength</tt> is not 1, 2, 4, or 8
     */
    LengthFieldPrepender(int lengthFieldLength);

    LengthFieldPrepender(int lengthFieldLength,
                         int checksumFieldLength,
                         const ChecksumFunction& checksum);

    /**
     * Creates a new instance.
     *
     * @param lengthFieldLength the length of the prepended length field.
     *                          Only 1, 2, 3, 4, and 8 are allowed.
     * @param lengthFieldLengthOffset
     *                          if <tt>0</tt>, the length of the prepended
     *                          length field is added to the value of the
     *                          prepended length field.
     *
     * @throws InvalidArgumentException
     *         if <tt>lengthFieldLength</tt> is not 1, 2, 4, or 8
     */
    LengthFieldPrepender(int lengthFieldLength,
                         const std::string& header);

    LengthFieldPrepender(int lengthFieldLength,
                         const std::string& header,
                         int checksumFieldLength,
                         const ChecksumFunction& checksum);

    LengthFieldPrepender(int lengthFieldLength,
                         int lengthFieldOffset,
                         const std::string& header);

    LengthFieldPrepender(int lengthFieldLength,
                         int lengthFieldOffset,
                         int lengthAdjustment,
                         const std::string& header);

    LengthFieldPrepender(int lengthFieldLength,
                         int lengthFieldOffset,
                         int lengthAdjustment,
                         int checksumFieldLength,
                         const std::string& header,
                         ChecksumFunction checksumFunction);

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual std::string toString() const { return "LengthFieldPrepender"; }

protected:
    virtual ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                                    const ChannelBufferPtr& msg,
                                    const ChannelBufferPtr& out);



private:
    void validateParameters();

    void writeHeader(const ChannelBufferPtr& msg, int contentLength, int headerPos);
    void preWriteHeader(const ChannelBufferPtr& msg, int contentLength, int headerPos);

    const ChannelBufferPtr& writeMessage(const ChannelBufferPtr& out,
                                         const ChannelBufferPtr& msg,
                                         int contentLength,
                                         int headerPos,
                                         uint32_t cs);

private:
    int  lengthFieldLength;
    int  lengthFieldOffset;
    int  lengthAdjustment;

    std::string header;

    int  checksumFieldLength;
    int  checksumCalcOffset;
    ChecksumFunction checksumFunction;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_LENGTHFIELDPREPENDER_H)

// Local Variables:
// mode: c++
// End:
