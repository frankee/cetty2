#if !defined(CETTY_BEANSTALK_PROTOCOL_BEANSTALKREPLYDECODER_H)
#define CETTY_BEANSTALK_PROTOCOL_BEANSTALKREPLYDECODER_H

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
/*
 * beanstalkReplyDecoder.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef BEANSTALKREPLYDECODER_H_
#define BEANSTALKREPLYDECODER_H_

#include <vector>
#include <cetty/util/StringPiece.h>
#include <cetty/handler/codec/BufferToMessageDecoder.h>
#include <cetty/beanstalk/protocol/BeanstalkReplyPtr.h>

namespace cetty {
namespace beanstalk {
namespace protocol {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::handler::codec;
using namespace protocol;

class BeanstalkReplyDecoder : private boost::noncopyable {
public:
    typedef BufferToMessageDecoder<BeanstalkReplyDecoder, BeanstalkReplyPtr> Decoder;

    typedef Decoder::Context Context;
    typedef Decoder::Handler Handler;
    typedef Decoder::HandlerPtr HandlerPtr;

public:
    BeanstalkReplyDecoder();
    ~BeanstalkReplyDecoder() {}

    void registerTo(Context& ctx) { decoder_.registerTo(ctx); }

private:
    BeanstalkReplyPtr decode(ChannelHandlerContext& ctx,
                             const ChannelBufferPtr& buffer);

    int findLineEnd(const StringPiece& bytes, int offset);

    int getIntValue(const StringPiece& bytes,
                    int offset,
                    int* value);

    int getStrValue(const StringPiece& bytes,
                    int offset,
                    StringPiece* str);

    BeanstalkReplyPtr reset();

private:
    int dataSize_;
    Decoder decoder_;
    BeanstalkReplyPtr reply_;
};

}
}
}

#endif /* BEANSTALKREPLYDECODER_H_ */


#endif //#if !defined(CETTY_BEANSTALK_PROTOCOL_BEANSTALKREPLYDECODER_H)

// Local Variables:
// mode: c++
// End:
