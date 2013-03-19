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
        FIRST_LINE,
        SECOND_LINE
    };

public:
    BeanstalkReplyDecoder(){ init(); }
    ~BeanstalkReplyDecoder() {}

    void registerTo(Context& ctx) { decoder.registerTo(ctx); }

private:
    void init();

    BeanstalkReplyPtr decode(ChannelHandlerContext& ctx,
                             const ReplayingDecoderBufferPtr& buffer,
                             int state);

    int indexOf(const StringPiece& bytes, int offset);

    void getResponse(StringPiece &bytes, std::string *response);
    void getInt(StringPiece &bytes, int *value, int offset);
    void getData(StringPiece &bytes, std::string *data, int offset);

    BeanstalkReplyPtr reset();

private:
    int dataLength;
    Decoder decoder;
    BeanstalkReplyPtr reply;
    CheckPointInvoker checkPoint;
};

}
}

#endif /* BEANSTALKREPLYDECODER_H_ */
