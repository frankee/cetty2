#if !defined(CETTY_CHANNEL_RECEIVEBUFFERSIZEPREDICTORFACTORY_H)
#define CETTY_CHANNEL_RECEIVEBUFFERSIZEPREDICTORFACTORY_H

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

#include <cetty/channel/ReceiveBufferSizePredictorFwd.h>
#include <cetty/channel/ReceiveBufferSizePredictorFactoryFwd.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace channel {

class ReceiveBufferSizePredictorFactory
    : public cetty::util::ReferenceCounter<ReceiveBufferSizePredictorFactory> {
public:
    virtual ~ReceiveBufferSizePredictorFactory() {}

    /**
     * Returns a newly created {@link ReceiveBufferSizePredictor}.
     */
    virtual ReceiveBufferSizePredictorPtr getPredictor() = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_RECEIVEBUFFERSIZEPREDICTORFACTORY_H)

// Local Variables:
// mode: c++
// End:

