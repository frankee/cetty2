#if !defined(CETTY_HANDLER_CODEC_FRAME_TOOLONGFRAMEEXCEPTION_H)
#define CETTY_HANDLER_CODEC_FRAME_TOOLONGFRAMEEXCEPTION_H

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

#include <cetty/util/Exception.h>

namespace cetty {
namespace handler {
namespace codec {
namespace frame {

using namespace cetty::util;

/**
 * An {@link Exception} which is thrown when the length of the frame
 * decoded by {@link DelimiterBasedFrameDecoder} is greater than the maximum.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.hidden
 */

CETTY_DECLARE_EXCEPTION(TooLongFrameException, RuntimeException)

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_FRAME_TOOLONGFRAMEEXCEPTION_H)

// Local Variables:
// mode: c++
// End:

