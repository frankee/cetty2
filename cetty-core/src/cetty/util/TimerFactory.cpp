/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/util/TimerFactory.h>

#include <boost/assert.hpp>
#include <cetty/util/Exception.h>

namespace cetty {
namespace util {

cetty::util::TimerFactoryPtr TimerFactory::factory = NULL;

void TimerFactory::setFactory(const TimerFactoryPtr& timerFactory) {
    if (!factory && timerFactory) {
        factory = timerFactory;
    }
    else {
        // log here.
    }
}

void TimerFactory::resetFactory(const TimerFactoryPtr& timerFactory) {
    if (timerFactory) {
        if (factory) {
            // log here.
            factory->stopTimers();
        }

        factory = timerFactory;
    }
}

void TimerFactory::resetFactory() {

}


TimerFactory& TimerFactory::getFactory() {
    BOOST_ASSERT(factory && "Factory has not been set.");

    if (!factory) {
        throw cetty::util::NullPointerException("timer factory");
    }

    return *factory;
}

bool TimerFactory::hasFactory() {
    return !!factory;
}

}
}