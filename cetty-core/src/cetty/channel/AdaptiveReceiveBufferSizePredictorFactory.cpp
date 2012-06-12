
/**
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

#include <cetty/channel/AdaptiveReceiveBufferSizePredictorFactory.h>
#include <cetty/channel/AdaptiveReceiveBufferSizePredictor.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

AdaptiveReceiveBufferSizePredictorFactory::AdaptiveReceiveBufferSizePredictorFactory()
    : minimum(AdaptiveReceiveBufferSizePredictor::DEFAULT_MINIMUM),
      initial(AdaptiveReceiveBufferSizePredictor::DEFAULT_INITIAL),
      maximum(AdaptiveReceiveBufferSizePredictor::DEFAULT_MAXIMUM) {
}

AdaptiveReceiveBufferSizePredictorFactory::AdaptiveReceiveBufferSizePredictorFactory(int minimum, int initial, int maximum) : minimum(minimum), initial(initial), maximum(maximum) {
    if (minimum <= 0) {
        throw InvalidArgumentException("minimum is less than zero.");
    }

    if (initial < minimum) {
        throw InvalidArgumentException("initial is less than zero.");
    }

    if (maximum < initial) {
        throw InvalidArgumentException("maximum is less than zero.");
    }
}

AdaptiveReceiveBufferSizePredictorFactory::~AdaptiveReceiveBufferSizePredictorFactory() {
}

cetty::channel::ReceiveBufferSizePredictorPtr AdaptiveReceiveBufferSizePredictorFactory::getPredictor() {
    return new AdaptiveReceiveBufferSizePredictor(minimum, initial, maximum);
}

}
}