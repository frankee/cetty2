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

#include <cetty/gearman/GearmanMessageChecker.h>

namespace cetty {
namespace gearman {

static bool checkCanDoMessage(const GearmanMessagePtr& msg) {
    if (msg->getParameters().size() == 1 && !msg->hasData()) {
        return true;
    }

    return false;
}

typedef bool (*MessageCheckFunction)(const GearmanMessagePtr&);

static const MessageCheckFunction CHECK_FUNCTIONS[] = {
    NULL,
    &checkCanDoMessage,
    NULL
};

static const int CHECK_FUNCTIONS_COUNT = sizeof(CHECK_FUNCTIONS) / sizeof(MessageCheckFunction);

bool GearmanMessageChecker::check(const GearmanMessagePtr& msg) {
    if (msg->getType() > CHECK_FUNCTIONS_COUNT) {
        return false;
    }

    MessageCheckFunction fun = CHECK_FUNCTIONS[msg->getType()];

    if (fun) {
        return fun(msg);
    }

    return false;
}

}
}