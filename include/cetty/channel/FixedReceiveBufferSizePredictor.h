#if !defined(CETTY_CHANNEL_FIXEDRECEIVEBUFFERSIZEPREDICTOR_H)
#define CETTY_CHANNEL_FIXEDRECEIVEBUFFERSIZEPREDICTOR_H

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

#include <cetty/channel/ReceiveBufferSizePredictor.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

/**
 * The {@link ReceiveBufferSizePredictor} that always yields the same buffer
 * size prediction.  This predictor ignores the feed back from the I/O thread.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class FixedReceiveBufferSizePredictor : public ReceiveBufferSizePredictor {
public:
    /**
     * Creates a new predictor that always returns the same prediction of
     * the specified buffer size.
     */
    FixedReceiveBufferSizePredictor(int bufferSize);

    int nextReceiveBufferSize() const;
    void previousReceiveBufferSize(int previousReceiveBufferSize);

private:
    int bufferSize;
};

}
}
#endif //#if !defined(CETTY_CHANNEL_FIXEDRECEIVEBUFFERSIZEPREDICTOR_H)

// Local Variables:
// mode: c++
// End:
