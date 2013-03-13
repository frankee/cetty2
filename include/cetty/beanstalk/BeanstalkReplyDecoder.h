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
#include <cetty/handler/codec/ReplayingDecoder.h>
#include <cetty/beanstalk/protocol/BeanstalkReply.h>

namespace cetty {
namespace beanstalk {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::handler::codec;
using namespace protocol;

class BeanstalkReplyDecoder : private boost::noncopyable {
public:
    typedef ReplayingDecoder<BeanstalkReplyDecoder, BeanstalkReplyPtr> Decoder;
    typedef Decoder::Context Context;
    typedef Decoder::Handler Handler;
    typedef Decoder::HandlerPtr HandlerPtr;

    typedef Decoder::CheckPointInvoker CheckPointInvoker;

public:
    enum State {
        READ_INITIAL,
        READ_BULK,
        READ_MULTI_BULK
    };

public:
    BeanstalkReplyDecoder(){ init(); }
    ~BeanstalkReplyDecoder() {}

    void registerTo(Context& ctx) { decoder.registerTo(ctx); }

private:
    void init() {
        checkPoint = decoder.checkPointInvoker();

        decoder.setInitialState(READ_INITIAL);
        decoder.setDecoder(boost::bind(&BeanstalkReplyDecoder::decode,
                                        this,
                                        _1,
                                        _2,
                                        _3));
    }

    BeanstalkReplyPtr decode(ChannelHandlerContext& ctx,
                             const ReplayingDecoderBufferPtr& buffer,
                             int state);

    void fail(ChannelHandlerContext& ctx, long frameLength);

    /**
     * Returns the number of bytes between the readerIndex of the haystack and
     * the first "\r\n" found in the haystack.  -1 is returned if no "\r\n" is
     * found in the haystack.
     */
    int indexOf(const StringPiece& bytes, int offset);

    BeanstalkReplyPtr reset();

private:

    BeanstalkReplyPtr reply;

    Decoder decoder;
    CheckPointInvoker checkPoint;
};

}
}

#endif /* BEANSTALKREPLYDECODER_H_ */
