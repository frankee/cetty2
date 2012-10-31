
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

#include <cetty/handler/codec/Delimiters.h>
#include <cetty/buffer/Unpooled.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;

const std::vector<ChannelBufferPtr>& Delimiters::nulDelimiter() {
    static std::vector<ChannelBufferPtr> buffers;

    if (buffers.size() == 0) {
        ChannelBufferPtr tmp = Unpooled::buffer(1);
        tmp->writeByte(0);
        buffers.push_back(tmp);
    }

    return buffers;
}

const std::vector<ChannelBufferPtr>& Delimiters::lineDelimiter() {
    static std::vector<ChannelBufferPtr> buffers;

    if (buffers.size() == 0) {
        ChannelBufferPtr cr = Unpooled::buffer(2);
        cr->writeByte('\r');
        cr->writeByte('\n');

        ChannelBufferPtr lf = Unpooled::buffer(1);
        lf->writeByte('\n');

        buffers.push_back(cr);
        buffers.push_back(lf);
    }

    return buffers;
}

}
}
}
