#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_ABSTRACTCODECEMBEDDER_H)
#define CETTY_HANDLER_CODEC_EMBEDDER_ABSTRACTCODECEMBEDDER_H

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

#include <deque>
#include <cetty/channel/ChannelSink.h>
#include <cetty/channel/ChannelUpstreamHandler.h>
#include <cetty/channel/DefaultChannelPipeline.h>
#include <cetty/handler/codec/embedder/CodecEmbedder.h>

namespace cetty {
namespace buffer {
class ChannelBufferFactory;
}
}

namespace cetty {
namespace handler {
namespace codec {
namespace embedder {

using namespace cetty::channel;
using namespace cetty::buffer;

/**
 * A skeletal {@link CodecEmbedder} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class AbstractCodecEmbedder : public CodecEmbedder {
public:
    virtual ~AbstractCodecEmbedder();

    virtual bool finish();

    virtual ChannelMessage poll();
    virtual const ChannelMessage& peek() const;
    virtual std::vector<ChannelMessage> pollAll();

    virtual int size() const;

protected:
    /**
     * Creates a new embedder whose pipeline is composed of the specified
     * handlers.
     */
    AbstractCodecEmbedder(const std::vector<ChannelHandlerPtr>& handlers);

    /**
     * Creates a new embedder whose pipeline is composed of the specified
     * handlers.
     *
     * @param bufferFactory the {@link ChannelBufferFactory} to be used when
     *                      creating a new buffer.
     */
    AbstractCodecEmbedder(ChannelBufferFactory* bufferFactory,
                          const std::vector<ChannelHandlerPtr>& handlers);

    /**
     * Returns the virtual {@link Channel} which will be used as a mock
     * during encoding and decoding.
     */
    const ChannelPtr& getChannel() {
        return *channel;
    }

    /**
     * Returns <tt>true</tt> if and only if the produce queue is empty and
     * therefore {@link #poll()} will return <tt>null</tt>.
     */
    bool empty() const {
        return productQueue.empty();
    }

private:
    void init(const std::vector<ChannelHandlerPtr>& handlers);
    void uninit();

    void fireInitialEvents();
    void configurePipeline(const std::vector<ChannelHandlerPtr>& handlers);

    class EmbeddedChannelSink : public cetty::channel::ChannelSink,
        public cetty::channel::ChannelUpstreamHandler {
    public:
        EmbeddedChannelSink(std::deque<ChannelMessage>& productQueue)
            : productQueue(productQueue) {
        }
        virtual ~EmbeddedChannelSink() {}

        virtual ChannelHandlerPtr clone() {
            return ChannelHandlerPtr(new EmbeddedChannelSink(productQueue));
        }
        virtual std::string toString() const { return "EmbeddedChannelSink"; }

        virtual void handleUpstream(ChannelHandlerContext& ctx, const ChannelEvent& e) {
        }

        virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
            handleMessageEvent(e);
        }

        virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
            //skip
        }

        virtual void writeCompleted(ChannelHandlerContext& ctx, const WriteCompletionEvent& e) {
            //skip
        }

        virtual void channelStateChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
            //skip
        }

        virtual void childChannelStateChanged(ChannelHandlerContext& ctx, const ChildChannelStateEvent& e) {
            //skip
        }

        virtual void eventSunk(const ChannelPipeline& pipeline, const ChannelEvent& e) {
            //skip
        }

        virtual void writeRequested(const ChannelPipeline& pipeline, const MessageEvent& e) {
            handleMessageEvent(e);
        }

        virtual void stateChangeRequested(const ChannelPipeline& pipeline, const ChannelStateEvent& e) {
        }

        virtual void exceptionCaught(const ChannelPipeline& pipeline,
                                     const ChannelEvent& e,
                                     const ChannelPipelineException& cause);

    private:
        void handleExceptionEvent(const ExceptionEvent& e);
        void handleMessageEvent(const MessageEvent& e);

    private:
        std::deque<ChannelMessage>& productQueue;
    };

    class EmbeddedChannelPipeline : public cetty::channel::DefaultChannelPipeline {
    public:
        EmbeddedChannelPipeline() {
        }
        virtual ~EmbeddedChannelPipeline() {}

    protected:
        virtual void notifyHandlerException(const ChannelEvent& evt,
                                            const Exception& e) const;
    };

private:
    Channel* channel;
    ChannelPipeline* pipeline;
    boost::intrusive_ptr<EmbeddedChannelSink> sink;

    std::deque<ChannelMessage> productQueue;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_ABSTRACTCODECEMBEDDER_H)

// Local Variables:
// mode: c++
// End:

