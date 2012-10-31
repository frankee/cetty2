#if !defined(CETTY_HANDLER_CODEC_DELIMITERS_H)
#define CETTY_HANDLER_CODEC_DELIMITERS_H

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

#include <vector>
#include <cetty/buffer/ChannelBufferPtr.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;

/**
 * A set of commonly used delimiters for {@link DelimiterBasedFrameDecoder}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */
class Delimiters {
public:
    /**
     * Returns a <tt>NUL (0x00)</tt> delimiter, which could be used for
     * Flash XML socket or any similar protocols.
     */
    static const std::vector<ChannelBufferPtr>& nulDelimiter();

    /**
     * Returns <tt>CR ('\r')</tt> and <tt>LF ('\n')</tt> delimiters, which could
     * be used for text-based line protocols.
     */
    static const std::vector<ChannelBufferPtr>& lineDelimiter();

private:
    Delimiters() {
        // Unused
    }
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_DELIMITERS_H)

// Local Variables:
// mode: c++
// End:
