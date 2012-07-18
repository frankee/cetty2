#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_ENCODEREMBEDDER_H)
#define CETTY_HANDLER_CODEC_EMBEDDER_ENCODEREMBEDDER_H

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

#include <cetty/handler/codec/embedder/AbstractCodecEmbedder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace embedder {

/**
 * A helper that wraps an encoder so that it can be used without doing actual
 * I/O in unit tests or higher level codecs.  For example, you can encode a
 * {@link String} into a Base64-encoded {@link ChannelBuffer} with
 * {@link Base64Encoder} and {@link StringEncoder} without setting up the
 * {@link ChannelPipeline} and other mock objects by yourself:
 * <pre>
 * String data = "foobar";
 *
 * {@link EncoderEmbedder}&lt;{@link ChannelBuffer}&gt; embedder = new {@link EncoderEmbedder}&lt;{@link ChannelBuffer}&gt;(
 *         new {@link Base64Encoder}(), new {@link StringEncoder}());
 *
 * embedder.offer(data);
 *
 * {@link ChannelBuffer} encoded = embedder.poll();
 * assert encoded.toString({@link CharsetUtil}.US_ASCII).equals("Zm9vYmFy");
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @see DecoderEmbedder
 */

class EncoderEmbedder : public AbstractCodecEmbedder {
public:
    /**
     * Creates a new embedder whose pipeline is composed of the specified
     * handlers.
     */
    EncoderEmbedder(const std::vector<ChannelHandlerPtr>& handlers)
        : AbstractCodecEmbedder(handlers) {
    }

    /**
     * Creates a new embedder whose pipeline is composed of the specified
     * handlers.
     *
     * @param bufferFactory the {@link ChannelBufferFactory} to be used when
     *                      creating a new buffer.
     */
    EncoderEmbedder(ChannelBufferFactory* bufferFactory,
                    const std::vector<ChannelHandlerPtr>& handlers)
        : AbstractCodecEmbedder(bufferFactory, handlers) {
    }

    virtual ~EncoderEmbedder() {}

    virtual bool offer(ChannelMessage& input);
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_EMBEDDER_ENCODEREMBEDDER_H)

// Local Variables:
// mode: c++
// End:
