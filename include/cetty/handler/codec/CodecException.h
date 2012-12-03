#if !defined(CETTY_HANDLER_CODEC_CODECEXCEPTION_H)
#define CETTY_HANDLER_CODEC_CODECEXCEPTION_H

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

#include <cetty/channel/ChannelException.h>

namespace cetty {
namespace handler {
namespace codec {

/**
 * An {@link Exception} which is thrown when a user calls an unsupported
 * operation on a {@link ChannelBuffer} in a {@link ReplayingDecoder}
 * implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

CETTY_DECLARE_EXCEPTION(CodecException, ::cetty::channel::ChannelException)

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_CODECEXCEPTION_H)

// Local Variables:
// mode: c++
// End:
